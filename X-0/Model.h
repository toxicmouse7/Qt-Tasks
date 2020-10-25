#pragma once
#include <vector>
#include <conio.h>
#include <ctime>
#include <Windows.h>
#include <QObject>

class ModelView;

enum player
{
	X,
	O,
	NONE,
	DRAW
};


class Model : QObject
{
    Q_OBJECT
private:
	int field_size;
	std::vector<std::vector<player>> board;
    player cur_player = X;
    bool is_with_bot = false;

	const int& GetSize();
	std::vector<std::vector<player>>& GetBoard();
    std::pair<short, short> MakeBotMove();
	player CheckWinner();
	player CheckHorizontal();
	player CheckVertical();
	player CheckDiagonal();
    void ChangePlayer();

	friend ModelView;
	
public:
    Model(int size, bool with_bot = false);

public slots:
    void MakeMove(std::pair<int, int> coords);

signals:
    void UpdateBoard(std::pair<int, int>);
    void EndGame(player);
	
};

