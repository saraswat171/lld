#include <iostream>

using namespace std;

class ATMState;     // Forward declaration of ATMState
class Card;         // Forward declaration of Card
class HasCardState; // Forward declaration of HasCardState

class CashInventory
{
private:
    int totalCash;

public:
    // CashInventory() : totalCash(0) {} // default constructor

    CashInventory(int cash = 0)
    {
        totalCash = cash;
    }

    bool hasSufficientCash(int amount)
    {
        return totalCash >= amount;
    }

    void dispenseCash(int amount)
    {
        totalCash -= amount;

        cout << "Dispensed: " << amount << endl;
    }
};

class BankAccount
{
private:
    int balance;

public:
    BankAccount(int bal)
    {
        balance = bal;
    }

    int getBalance()
    {
        return balance;
    }

    bool withdraw(int amount)
    {

        if (balance >= amount)
        {
            balance -= amount;
            return true;
        }

        return false;
    }
};

class Card
{
private:
    string cardNumber;
    int pin;

public:
    BankAccount *bankAccount;

    Card(string number, int p, BankAccount *acc)
    {
        cardNumber = number;
        pin = p;
        bankAccount = acc;
    }

    bool validatePin(int enteredPin)
    {
        return pin == enteredPin;
    }
};

class ATM
// ATM class maintains a reference to the current state and delegates state-specific behavior to the current state object.
// It also holds references to the current card and cash inventory.
{
private:
    ATMState *currentState;

public:
    Card *currentCard;
    CashInventory cashInventory;

    ATM(ATMState *state)
    {
        currentState = state;
    }

    void setState(ATMState *state)
    {
        // delete currentState; // Clean up the old state to prevent memory leaks
        currentState = state;
    }

    ATMState *getState()
    {
        return currentState;
    }
};

class ATMState
// below are state interface methods. Each state will override the relevant methods and provide implementation. For other methods,
// it will use the default implementation which is to print "Operation not allowed".
{
public:
    virtual void insertCard(ATM *atm)
    {
        cout << "Operation not allowed\n";
    }

    virtual void authenticatePin(ATM *atm, int pin)
    {
        cout << "Operation not allowed\n";
    }

    virtual void selectOperation(ATM *atm, int choice)
    {
        cout << "Operation not allowed\n";
    }

    virtual void cashWithdrawal(ATM *atm, int amount)
    {
        cout << "Operation not allowed\n";
    }

    virtual void displayBalance(ATM *atm)
    {
        cout << "Operation not allowed\n";
    }

    virtual void ejectCard(ATM *atm)
    {
        cout << "Card ejected\n";
    }

    virtual ~ATMState() {}
};

class IdleState : public ATMState
{
public:
    void insertCard(ATM *atm) override;
};

class HasCardState : public ATMState
{
public:
    void authenticatePin(ATM *atm, int pin) override;

    void ejectCard(ATM *atm) override;
};

enum TransactionType
{
    CASH_WITHDRAWAL,
    BALANCE_CHECK
};

class CashWithdrawalState : public ATMState
{

public:
    void cashWithdrawal(ATM *atm, int amount) override
    {

        BankAccount *account =
            atm->currentCard->bankAccount;

        if (!atm->cashInventory.hasSufficientCash(amount))
        {
            cout << "ATM has insufficient cash\n";
            return;
        }

        if (!account->withdraw(amount))
        {
            cout << "Insufficient bank balance\n";
            return;
        }

        atm->cashInventory.dispenseCash(amount);

        cout << "Withdrawal successful\n";

        ejectCard(atm);
    }

    void ejectCard(ATM *atm) override
    {

        cout << "Card returned\n";
        atm->currentCard = nullptr;
        atm->setState(new IdleState());
    }
};

class CheckBalanceState : public ATMState
{

public:
    void displayBalance(ATM *atm) override
    {

        cout << "Balance: "
             << atm->currentCard
                    ->bankAccount
                    ->getBalance()
             << endl;

        ejectCard(atm);
    }

    void ejectCard(ATM *atm) override
    {

        cout << "Card returned\n";
        atm->currentCard = nullptr;
        atm->setState(new IdleState());
    }
};

class SelectOperationState : public ATMState
{
public:
    void selectOperation(ATM *atm, int choice) override
    {

        switch (choice)
        {

        case CASH_WITHDRAWAL:
            atm->setState(new CashWithdrawalState());
            break;

        case BALANCE_CHECK:
            atm->setState(new CheckBalanceState());
            break;

        default:
            cout << "Invalid operation\n";
        }
    }
};

void IdleState::insertCard(ATM *atm)
{
    cout << "Card inserted\n";

    atm->setState(new HasCardState());
}

void HasCardState::authenticatePin(ATM *atm, int pin)
{
    if (atm->currentCard->validatePin(pin))
    {
        cout << "PIN validated\n";

        atm->setState(new SelectOperationState());
    }
    else
    {
        cout << "Invalid PIN\n";
        ejectCard(atm);
    }
}

void HasCardState::ejectCard(ATM *atm)
{
    cout << "Card ejected\n";

    atm->currentCard = nullptr;

    atm->setState(new IdleState());
}

class ATMStateFactory
{

public:
    static ATMState *createIdleState()
    {
        return new IdleState();
    }

    static ATMState *createHasCardState()
    {
        return new HasCardState();
    }
};

int main()
{
    BankAccount account(1000);
    Card card("1234-5678-9012-3456", 1234, &account);

    ATM atm(ATMStateFactory::createIdleState());
    atm.cashInventory = CashInventory(1400);

    atm.currentCard = &card;
    atm.getState()->insertCard(&atm);
    atm.getState()->authenticatePin(&atm, 1234);
    atm.getState()->selectOperation(&atm, CASH_WITHDRAWAL);
    atm.getState()->cashWithdrawal(&atm, 200);

    return 0;
}
