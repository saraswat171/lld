#include <iostream>
#include <string>
#include <vector>
#include <memory>
using namespace std;

enum class Currency {
 INR,
 DOLLAR,
 EURO
};


class User {
private:
    int id;
    string name;

public:
    User(int id, const string& name)
        : id(id), name(name) {}

    int getId() const {
        return id;
    }

    const string& getName() const {
        return name;
    }
};

class ExpenseParticipant {
   private: 
    User user;
    double amount;
 
   public:
    User getUser(){return user;}

    double getAmount(){return amount;}

   void updateAmount(double amount) {
         
    }
  
};

class ExpenseParticipant {
private:
    shared_ptr<User> user;
    double amount;

public:
    ExpenseParticipant(shared_ptr<User> user, double amount)
        : user(move(user)), amount(amount) {}

    shared_ptr<User> getUser() const {
        return user;
    }

    double getAmount() const {
        return amount;
    }

    void updateAmount(double newAmount) {
        amount = newAmount;
    }
};


class Balance {
  private:
    User creditor;
    User debtor;
    double amount;

  public:

    User getCreditor(){ return creditor;}  
    User getDebtor(){return debtor;}

    double amount() {return amount;}

    double increaseAmount(double newAmount){
        amount = amount+ newAmount;
    }

     double decreaseAmount(double newAmount){
        amount = amount - newAmount;
    }

    settle()



};