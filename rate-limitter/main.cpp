/*

    * Rate Limiter Implementation
    *
    * This code implements a rate limiter using the token bucket algorithm.
    * It allows for controlling the rate of requests based on a specified capacity
    * and refill rate. The implementation is thread-safe and can handle concurrent
    * requests from multiple threads.
    *
    * The main components of the implementation are:
    * - RateLimitConfig: A structure that holds the configuration for the rate limiter,
    *   including the capacity and refill rate.
    * - Bucket: A structure that represents a token bucket, containing the current
    *   number of tokens and the last refill time. It also includes a mutex for
    *   thread safety.
    * - RateLimitStrategy: An abstract base class that defines the interface for
    *   different rate limiting strategies.
    * - TokenBucketStrategy: A concrete implementation of the RateLimitStrategy
    *   that uses the token bucket algorithm to determine if a request is allowed.
    * - RateLimiter: A class that encapsulates the rate limiting logic and provides
    *   an interface for checking if a request is allowed based on a key.
    *
    * The main function demonstrates how to use the RateLimiter with multiple threads,
    * simulating concurrent requests and counting how many were allowed or rejected.
    *
*/

/*

#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>

using namespace std;

class RateLimiter {
    int windowSize;
    int limit;

    unordered_map<string, queue<int>> userRequests;

public:

    RateLimiter(int limit, int windowInSeconds) {
        this->limit = limit;
        this->windowSize = windowInSeconds;
    }


    bool allowRequest(const string& userId, int timestampInSeconds) {

        queue<int>& requests = userRequests[userId];

        // Remove requests which are outside the current window
        while (!requests.empty() &&
               timestampInSeconds - requests.front() >= windowSize) {
            requests.pop();0
        }

        // User has already reached the limit
        if (requests.size() >= limit) {
            return false;
        }

        // Allow this request
        requests.push(timestampInSeconds);

        return true;
    }
};

int main() {

    // Allow maximum 3 requests per 10-second window
    RateLimiter limiter(3, 10);

    cout << boolalpha;

    cout << limiter.allowRequest("User_101", 100) << "\n"; // true
    cout << limiter.allowRequest("User_101", 102) << "\n"; // true
    cout << limiter.allowRequest("User_101", 105) << "\n"; // true
    cout << limiter.allowRequest("User_101", 108) << "\n"; // false

    // 100 is outside [101, 111], so it gets removed
    cout << limiter.allowRequest("User_101", 111) << "\n"; // true

    // Different user has a separate rate limit
    cout << limiter.allowRequest("User_202", 111) << "\n"; // true
    cout << limiter.allowRequest("User_202", 112) << "\n"; // true

    return 0;
}

*/

#include <algorithm>
#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;

struct RateLimitConfig
{
    double capacity;
    double refillRate; // tokens per second
};

struct Bucket
{
    double tokens;
    TimePoint lastRefill;

    // Protects this particular bucket.
    std::mutex mutex;

    Bucket(double tokens, TimePoint lastRefill)
        : tokens(tokens),
          lastRefill(lastRefill) {}
};

class RateLimitStrategy
{
public:
    virtual bool allow(const std::string &key) = 0;

    virtual ~RateLimitStrategy() = default;  //If a class is going to be used as a polymorphic base class,
                                            // give it a virtual destructor so derived objects 
                                           //  are destroyed correctly through the base pointer.
};

class TokenBucketStrategy : public RateLimitStrategy
{

private:
    RateLimitConfig config;

    // Protects the unordered_map itself.
    std::mutex mapMutex; //C++ unordered_map is not safe for concurrent modification. Map mutex protects finding/managing the buckets.

    // One bucket per key.
    std::unordered_map<
        std::string,
        std::unique_ptr<Bucket>>
        buckets;

    Bucket &getOrCreateBucket(const std::string &key)
    {

        std::lock_guard<std::mutex> lock(mapMutex);

        auto it = buckets.find(key);

        if (it != buckets.end()) // if bucket exists for the key, return it
        {
            return *(it->second);  // dereference the unique_ptr to get the Bucket object
        }

        auto bucket = std::make_unique<Bucket>(
            config.capacity,
            Clock::now());

        Bucket &bucketRef = *bucket; // store a reference to the newly created bucket before moving it into the map

        buckets.emplace(
            key,
            std::move(bucket)); // move the unique_ptr into the map to transfer ownership

        return bucketRef;
    }

public:
    explicit TokenBucketStrategy(
        const RateLimitConfig &config)
        : config(config) {}

    bool allow(const std::string &key) override
    {

        // Get/create the bucket.
        Bucket &bucket = getOrCreateBucket(key);

        // Lock only this user's bucket.
        std::lock_guard<std::mutex> lock(bucket.mutex); //Bucket mutex protects modifying the state inside one particular bucket.

        auto now = Clock::now();

        // ---------------------------------------------
        // Calculate elapsed time
        // ---------------------------------------------

        double elapsed =
            std::chrono::duration<double>(
                now - bucket.lastRefill)
                .count();

        // ---------------------------------------------
        // Refill tokens
        // ---------------------------------------------

        bucket.tokens = std::min(
            config.capacity,
            bucket.tokens +
                elapsed * config.refillRate);

        bucket.lastRefill = now;

        // ---------------------------------------------
        // Check whether request is allowed
        // ---------------------------------------------

        if (bucket.tokens < 1.0)
        {
            return false;
        }

        // ---------------------------------------------
        // Consume one token
        // ---------------------------------------------

        bucket.tokens -= 1.0;

        return true;
    }

};

class RateLimiter
{

private:
    std::unique_ptr<RateLimitStrategy> strategy; //unique_ptr means "I own this object, and C++ should clean it up for me."

public:
     RateLimiter(
        std::unique_ptr<RateLimitStrategy> strategy)
        : strategy(std::move(strategy)) {} // without this strategy(std::move(strategy) parameter strategy is not being stored in the member variable.

    bool allow(const std::string &key)
    {
        return strategy->allow(key);
    }
};

int main()
{

    RateLimitConfig config{
        5,  // capacity
        0.0 // refill rate
    };

    RateLimiter limiter(
        std::make_unique<TokenBucketStrategy>(config));

    std::atomic<int> allowed{0};
    std::atomic<int> rejected{0};

    std::vector<std::thread> threads;

    for (int i = 0; i < 20; ++i)
    {

        threads.emplace_back([&]()
                             {

            if (limiter.allow("user1")) {
                ++allowed;
            }
            else {
                ++rejected;
            } });
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    std::cout
        << "Allowed: "
        << allowed
        << '\n';

    std::cout
        << "Rejected: "
        << rejected
        << '\n';
}
