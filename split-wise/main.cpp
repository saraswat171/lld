#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <unordered_map>
using namespace std;

enum class Currency
{
    INR,
    DOLLAR,
    EURO
};

class User
{
private:
    int id;
    string name;

public:
    User(int id, const string &name)
        : id(id), name(name) {}

    int getId() const
    {
        return id;
    }

    const string &getName() const
    {
        return name;
    }
};

class ExpenseParticipant
{
private:
    shared_ptr<User> user;
    double amount;

public:
    ExpenseParticipant(shared_ptr<User> user, double amount)
        : user(move(user)), amount(amount) {}

    shared_ptr<User> getUser() const
    {
        return user;
    }

    double getAmount() const
    {
        return amount;
    }

    void updateAmount(double newAmount)
    {
        amount = newAmount;
    }
};

class Balance
{
private:
    shared_ptr<User> creditor;
    shared_ptr<User> debtor;
    double amount;

public:
    Balance(
        shared_ptr<User> creditor,
        shared_ptr<User> debtor,
        double amount)
        : creditor(std::move(creditor)),
          debtor(std::move(debtor)),
          amount(amount)
    {
    }
    shared_ptr<User> getCreditor() const { return creditor; }
    shared_ptr<User> getDebtor() const { return debtor; }

    double getAmount() const { return amount; }

    void increaseAmount(double newAmount)
    {
        amount = amount + newAmount;
    }

    void decreaseAmount(double newAmount)
    {
        amount = amount - newAmount;
    }
};

class Expense
{
private:
    int id;
    shared_ptr<User> payer;
    double amount;
    vector<ExpenseParticipant> participants;
    string description;

public:
    Expense(int id,
            shared_ptr<User> payer,
            double amount,
            vector<ExpenseParticipant> participants,
            const string &description)
        : id(id),
          payer(move(payer)),
          amount(amount),
          participants(move(participants)),
          description(description) {}

    int getId() const
    {
        return id;
    }

    shared_ptr<User> getPayer() const
    {
        return payer;
    }

    double getAmount() const
    {
        return amount;
    }

    const vector<ExpenseParticipant> &getParticipants() const
    {
        return participants;
    }

    const string &getDescription() const
    {
        return description;
    }

    void updateAmount(double newAmount)
    {
        amount = newAmount;
    }

    void updateDescription(const string &newDescription)
    {
        description = newDescription;
    }

    void updateParticipants(
        const vector<ExpenseParticipant> &newParticipants)
    {
        participants = newParticipants;
    }
};

class Group
{
private:
    int id;
    string name;
    Currency currency;

    vector<shared_ptr<User>> members;
    vector<shared_ptr<Expense>> expenses;
    vector<Balance> balances;

    int nextExpenseId = 1;

private:
    Balance *findBalance(
        const shared_ptr<User> &creditor,
        const shared_ptr<User> &debtor)
    {
        Balance *balance = nullptr;
        for (auto &b : balances)
        {
            if (b.getCreditor()->getId() == creditor->getId() && b.getDebtor()->getId() == debtor->getId())
            {
                balance = &b;
                break;
            }
        }
        return balance;
    }

    void updateBalance(
        const shared_ptr<User> &creditor,
        const shared_ptr<User> &debtor,
        double amount)
    {
        if (Balance *same = findBalance(creditor, debtor))
        {
            same->increaseAmount(amount);
            return;
        }

        if (Balance *opposite = findBalance(debtor, creditor))
        {
            double existing = opposite->getAmount();

            if (existing > amount)
            {
                opposite->decreaseAmount(amount);
            }
            else if (existing == amount)
            {
                removeBalance(debtor, creditor);
            }
            else
            {
                double remaining = amount - existing;

                removeBalance(debtor, creditor);

                balances.emplace_back(
                    creditor,
                    debtor,
                    remaining);
            }

            return;
        }

        balances.emplace_back(
            creditor,
            debtor,
            amount);
    }

    void applyBalanceChange(
        const shared_ptr<Expense> &expense)
    {
        auto payer = expense->getPayer();

        for (const auto &participant : expense->getParticipants())
        {
            auto user = participant.getUser();

            if (user->getId() == payer->getId())
                continue;

            updateBalance(
                payer,
                user,
                participant.getAmount());
        }
    }

public:
    Group(
        int id,
        const string &name,
        Currency currency)
        : id(id),
          name(name),
          currency(currency)
    {
    }

    bool isMember(
        const shared_ptr<User> &user) const
    {

        for (const auto &member : members)
        {
            if (member->getId() == user->getId())
            {
                return true;
            }
        }
        return false;
    };

    int getId() const
    {
        return id;
    };

    void addMember(
        const shared_ptr<User> &user)
    {
        if (!isMember(user))
        {
            members.push_back(user);
        }
        else
        {
            throw invalid_argument("User is already a member of the group.");
        }
    };

    int getNextExpenseId()
    {
        return nextExpenseId++;
    };

    void addExpense(
        const shared_ptr<Expense> &expense)
    {
        expenses.push_back(expense);
        applyBalanceChange(expense);
    };

    const vector<Balance> &getBalances() const
    {
        return balances;
    }

    void editExpense(
        int expenseId,
        const shared_ptr<User> &payer,
        double amount,
        const vector<ExpenseParticipant> &participants,
        const string &description)
    {
        for (auto &expense : expenses)
        {
            if (expense->getId() == expenseId)
            {
                expense->updateAmount(amount);
                expense->updateDescription(description);
                expense->updateParticipants(participants);
                applyBalanceChange(expense);
                return;
            }
        }
        throw invalid_argument("Expense not found.");
    };

    void removeExpense(
        int expenseId)
    {
        for (auto it = expenses.begin(); it != expenses.end(); ++it)
        {
            if ((*it)->getId() == expenseId)
            {
                expenses.erase(it);
                return;
            }
        }
        throw invalid_argument("Expense not found.");
    };

    void removeBalance(
        const shared_ptr<User> &creditor,
        const shared_ptr<User> &debtor)
    {
        for (auto it = balances.begin(); it != balances.end(); ++it)
        {
            if (it->getCreditor()->getId() == creditor->getId() && it->getDebtor()->getId() == debtor->getId())
            {
                balances.erase(it);
                return;
            }
        }
        throw invalid_argument("Balance not found.");
    };
};

class ExpenseManager
{
public:
    void addExpense(
        Group &group,
        shared_ptr<User> payer,
        double amount,
        const vector<ExpenseParticipant> &participants,
        const string &description)
    {
        if (!validateExpense(group, payer, amount, participants))
            throw invalid_argument("Invalid expense.");

        int expenseId = group.getNextExpenseId();

        auto expense = make_shared<Expense>(
            expenseId,
            payer,
            amount,
            participants,
            description);

        group.addExpense(expense);
    }

    void editExpense(
        Group &group,
        int expenseId,
        shared_ptr<User> payer,
        double amount,
        const vector<ExpenseParticipant> &participants,
        const string &description)
    {
        if (!validateExpense(group, payer, amount, participants))
            throw invalid_argument("Invalid expense.");

        group.editExpense(
            expenseId,
            payer,
            amount,
            participants,
            description);
    }

    void deleteExpense(
        Group &group,
        int expenseId)
    {
        group.removeExpense(expenseId);
    }

private:
    bool validateExpense(
        const Group &group,
        const shared_ptr<User> &payer,
        double amount,
        const vector<ExpenseParticipant> &participants)
    {
        if (amount <= 0)
            return false;

        if (!payer)
            return false;

        if (participants.empty())
            return false;

        if (!group.isMember(payer))
            return false;

        double total = 0;

        for (const auto &participant : participants)
        {
            if (!participant.getUser())
                return false;

            if (!group.isMember(participant.getUser()))
                return false;

            total += participant.getAmount();
        }

        return total == amount;
    }
};

class GroupManager
{
private:
    vector<shared_ptr<Group>> groups;

public:
    shared_ptr<Group> createGroup(
        int id,
        const string &name,
        Currency currency)
    {
        for (const auto &group : groups)
        {
            if (group->getId() == id)
            {
                throw invalid_argument("Group id already exists.");
            }
        }

        auto group = make_shared<Group>(
            id,
            name,
            currency);

        groups.push_back(group);

        return group;
    }

    void deleteGroup(int groupId)
    {
        for (auto it = groups.begin(); it != groups.end(); ++it)
        {
            if ((*it)->getId() == groupId)
            {
                groups.erase(it);
                return;
            }
        }

        throw invalid_argument("Group not found.");
    }

    shared_ptr<Group> getGroup(int groupId) const
    {
        for (const auto &group : groups)
        {
            if (group->getId() == groupId)
            {
                return group;
            }
        }

        throw invalid_argument("Group not found.");
    }

    const vector<shared_ptr<Group>> &getGroups() const
    {
        return groups;
    }
};

class SettlementSuggestion
{
private:
    shared_ptr<User> from;
    shared_ptr<User> to;
    double amount;

public:
    SettlementSuggestion(
        shared_ptr<User> from,
        shared_ptr<User> to,
        double amount)
        : from(move(from)),
          to(move(to)),
          amount(amount)
    {
    }

    shared_ptr<User> getFrom() const
    {
        return from;
    }

    shared_ptr<User> getTo() const
    {
        return to;
    }

    double getAmount() const
    {
        return amount;
    }
};

class SettlementOptimizer
{
private:
    struct NetBalance
    {
        shared_ptr<User> user;
        double amount;
    };

public:
    vector<SettlementSuggestion> generateSettlementSuggestions(
        const Group &group)
    {
        unordered_map<int, NetBalance> netBalances;

        for (const Balance &balance : group.getBalances())
        {
            auto creditor = balance.getCreditor();
            auto debtor = balance.getDebtor();
            double amount = balance.getAmount();

            if (netBalances.find(creditor->getId()) == netBalances.end())
            {
                netBalances[creditor->getId()] =
                    {
                        creditor,
                        0};
            }

            if (netBalances.find(debtor->getId()) == netBalances.end())
            {
                netBalances[debtor->getId()] =
                    {
                        debtor,
                        0};
            }

            netBalances[creditor->getId()].amount += amount;
            netBalances[debtor->getId()].amount -= amount;
        }

        vector<NetBalance> creditors;
        vector<NetBalance> debtors;

        for (auto &entry : netBalances)
        {
            if (entry.second.amount > 0)
            {
                creditors.push_back(entry.second);
            }
            else if (entry.second.amount < 0)
            {
                entry.second.amount *= -1;
                debtors.push_back(entry.second);
            }
        }

        sort(
            creditors.begin(),
            creditors.end(),
            [](const NetBalance &a, const NetBalance &b)
            {
                return a.amount > b.amount;
            });

        sort(
            debtors.begin(),
            debtors.end(),
            [](const NetBalance &a, const NetBalance &b)
            {
                return a.amount > b.amount;
            });

        vector<SettlementSuggestion> suggestions;

        size_t i = 0;
        size_t j = 0;

        while (i < creditors.size() && j < debtors.size())
        {
            double settleAmount =
                min(
                    creditors[i].amount,
                    debtors[j].amount);

            suggestions.emplace_back(
                debtors[j].user,
                creditors[i].user,
                settleAmount);

            creditors[i].amount -= settleAmount;
            debtors[j].amount -= settleAmount;

            if (creditors[i].amount == 0)
                i++;

            if (debtors[j].amount == 0)
                j++;
        }

        return suggestions;
    }
};

int main()
{
    // Managers
    GroupManager groupManager;
    ExpenseManager expenseManager;
    SettlementOptimizer settlementOptimizer;

    // Users
    auto alice = make_shared<User>(1, "Alice");
    auto bob = make_shared<User>(2, "Bob");
    auto charlie = make_shared<User>(3, "Charlie");

    // Create Group
    auto trip = groupManager.createGroup(
        1,
        "Goa Trip",
        Currency::INR);

    // Add Members
    trip->addMember(alice);
    trip->addMember(bob);
    trip->addMember(charlie);
    // trip->addMember(alice);  GIVES ERROR: User is already a member of the group.


    // ---------------- Expense 1 ----------------
    // Alice paid 900 for everyone

    vector<ExpenseParticipant> participants1;

    participants1.emplace_back(alice, 300);
    participants1.emplace_back(bob, 300);
    participants1.emplace_back(charlie, 300);

    expenseManager.addExpense(
        *trip,
        alice,
        900,
        participants1,
        "Dinner");

    // ---------------- Expense 2 ----------------
    // Bob paid 300 for Alice and Bob

    vector<ExpenseParticipant> participants2;

    participants2.emplace_back(alice, 150);
    participants2.emplace_back(bob, 150);

    //   participants2.emplace_back(alice, 150);
    // participants2.emplace_back(charlie,   150);

    expenseManager.addExpense(
        *trip,
        bob,
        300,
        participants2,
        "Taxi");

    // ---------------- Print Current Balances ----------------

    cout << "Current Balances\n";
    cout << "----------------\n";

    for (const auto &balance : trip->getBalances())
    {
        cout
            << balance.getDebtor()->getName()
            << " owes "
            << balance.getCreditor()->getName()
            << " : "
            << balance.getAmount()
            << endl;
    }

    cout << endl;

    // ---------------- Settlement Suggestions ----------------

    auto suggestions =
        settlementOptimizer.generateSettlementSuggestions(*trip);

    cout << "Settlement Suggestions\n";
    cout << "----------------------\n";

    for (const auto &suggestion : suggestions)
    {
        cout
            << suggestion.getFrom()->getName()
            << " pays "
            << suggestion.getTo()->getName()
            << " : "
            << suggestion.getAmount()
            << endl;
    }

    return 0;
}