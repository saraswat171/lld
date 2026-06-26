#include <iostream>
#include <string>
#include <vector>
using namespace std;

enum class Symbol
{
    EMPTY,
    X,
    O
};

enum class GameStatus
{
    IN_PROGRESS,
    WIN,
    DRAW
};

class Move
{
private:
    int row;
    int col;
    Symbol symbol;

public:
    Move(int row, int col, Symbol symbol) : row(row), col(col), symbol(symbol) {}
    int getRow() const { return row; }
    int getCol() const { return col; }
    Symbol getSymbol() const { return symbol; }
};

class Cell
{
private:
    Symbol symbol;

public:
    Cell() : symbol(Symbol::EMPTY) {}

    bool isOccupied() const
    {
        return symbol != Symbol::EMPTY;
    }

    Symbol getSymbol() const
    {
        return symbol;
    }

    void setSymbol(Symbol symbol)
    {
        this->symbol = symbol;
    }
};

class Player
{

public:
    string name;
    Symbol symbol;

public:
    Player(string name, Symbol symbol) : name(name), symbol(symbol) {}

    string getName() const
    {
        return name;
    }

    Symbol getSymbol() const
    {
        return symbol;
    }

    Move chooseMove()
    {
        int row, col;
        cout << name << " (" << (symbol == Symbol::X ? 'X' : 'O') << "), enter your move (row and column): ";
        cin >> row >> col;
        return Move(row, col, symbol);
    }
};

class Board
{
private:
    int size;
    vector<vector<Cell>> grid;

public:
    Board(int size) : size(size), grid(size, vector<Cell>(size)) {}

    int getSize() const
    {
        return size;
    }

    bool isValidMove(const Move &move) const
    {
        int row = move.getRow();
        int col = move.getCol();

        if (row < 0 || row >= size ||
            col < 0 || col >= size)
        {
            return false;
        }

        return !grid[row][col].isOccupied();
    }

    void placeMove(const Move &move)
    {
        grid[move.getRow()][move.getCol()].setSymbol(move.getSymbol());
    }

    const Cell &getCell(int row, int col) const
    {
        return grid[row][col];
    }

    bool isBoardFull() const
    {
        for (const auto &row : grid)
        {
            for (const auto &cell : row)
            {
                if (!cell.isOccupied())
                    return false;
            }
        }

        return true;
    }

    void display() const
    {
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                Symbol symbol = grid[i][j].getSymbol();

                char ch = ' ';

                if (symbol == Symbol::X)
                    ch = 'X';
                else if (symbol == Symbol::O)
                    ch = 'O';

                cout << ch;

                if (j != size - 1)
                    cout << " | ";
            }

            cout << endl;

            if (i != size - 1)
            {
                cout << string(size * 4 - 3, '-');
                cout << endl;
            }
        }
    }
};

class WinningStrategy
{
public:
    virtual bool checkWin(const Board &board, const Move &move) = 0;

    virtual ~WinningStrategy() = default;
};

class RowWinningStrategy : public WinningStrategy
{
    bool checkWin(const Board &board,
                  const Move &move) override
    {
        int row = move.getRow();
        Symbol playerSymbol = move.getSymbol();

        for (int col = 0; col < board.getSize(); col++)
        {
            if (board.getCell(row, col).getSymbol() != playerSymbol)
            {
                return false;
            }
        }

        return true;
    }
};

class ColumnWinningStrategy : public WinningStrategy
{
    bool checkWin(const Board &board,
                  const Move &move) override
    {
        int col = move.getCol();
        Symbol playerSymbol = move.getSymbol();

        for (int row = 0; row < board.getSize(); row++)
        {
            if (board.getCell(row, col).getSymbol() != playerSymbol)
            {
                return false;
            }
        }
        return true;
    }
};

class DiagonalWinningStrategy : public WinningStrategy
{
public:
    bool checkWin(const Board &board,
                  const Move &move) override
    {
        int row = move.getRow();
        int col = move.getCol();
        Symbol symbol = move.getSymbol();

        int size = board.getSize();

        if (row == col)
        {
            bool win = true;

            for (int i = 0; i < size; i++)
            {
                if (board.getCell(i, i).getSymbol() != symbol)
                {
                    win = false;
                    break;
                }
            }

            if (win)
                return true;
        }

        if (row + col == size - 1)
        {
            bool win = true;

            for (int i = 0; i < size; i++)
            {
                if (board.getCell(i, size - 1 - i).getSymbol() != symbol)
                {
                    win = false;
                    break;
                }
            }

            if (win)
                return true;
        }

        return false;
    }
};

class Game
{
private:
    Board board;

    vector<Player> players;

    vector<WinningStrategy *> strategies;

    int currentPlayerIndex;

    GameStatus gameStatus;

public:
    Game(int boardSize,
         const vector<Player> &players,
         const vector<WinningStrategy *> &strategies)
        : board(boardSize),
          players(players),
          strategies(strategies),
          currentPlayerIndex(0),
          gameStatus(GameStatus::IN_PROGRESS)
    {
    }

    void startGame()
    {
        while (gameStatus == GameStatus::IN_PROGRESS)
        {
            board.display();

            makeMove();
        }

        board.display();

        if (gameStatus == GameStatus::WIN)
        {
            cout << getCurrentPlayer().getName() << " Wins!" << endl;
        }
        else
        {
            cout << "Game Draw!" << endl;
        }
    }

private:
    void makeMove()
    {
        Player &player = getCurrentPlayer();

        while (true)
        {
            Move move = player.chooseMove();

            if (!board.isValidMove(move))
            {
                cout << "Invalid Move! Please try again.\n";
                continue;
            }

            board.placeMove(move);

            checkGameStatus(move);

            if (gameStatus == GameStatus::IN_PROGRESS)
            {
                trackTurns();
            }

            break;
        }
    }

    void checkGameStatus(const Move &move)
    {
        for (auto strategy : strategies)
        {
            if (strategy->checkWin(board, move))
            {
                gameStatus = GameStatus::WIN;
                return;
            }
        }

        if (board.isBoardFull())
        {
            gameStatus = GameStatus::DRAW;
        }
    }

    void trackTurns()
    {
        currentPlayerIndex =
            (currentPlayerIndex + 1) % players.size();
    }

    Player &getCurrentPlayer()
    {
        return players[currentPlayerIndex];
    }
};

int main()
{
    vector<Player> players = {
        Player("Alice", Symbol::X),
        Player("Bob", Symbol::O)};

    vector<WinningStrategy *> strategies;

    strategies.push_back(new RowWinningStrategy());
    strategies.push_back(new ColumnWinningStrategy());
    strategies.push_back(new DiagonalWinningStrategy());

    Game game(3, players, strategies);

    game.startGame();

    return 0;
}