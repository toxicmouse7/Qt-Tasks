#include "Model.h"

Model::Model(int size, bool with_bot) : field_size(size)
{
	if (field_size > 2)
	{
		std::vector<player> temp(size, NONE);
        is_with_bot = with_bot;
	
		for (int i = 0; i < field_size; ++i)
		{
			board.push_back(temp);
		}
	}
}

const int& Model::GetSize()
{
	return field_size;
}

void Model::MakeMove(std::pair<int, int> coords)
{
    if (board[coords.first][coords.second] == NONE)
    {
        player winner;
        board[coords.first][coords.second] = cur_player;
        ChangePlayer();
        emit UpdateBoard(coords);
        if ((winner = CheckWinner()) != NONE)
        {
            emit EndGame(winner);
            return;
        }

        if (is_with_bot)
        {
            std::pair<short, short> bot_move = MakeBotMove();
            ChangePlayer();
            emit UpdateBoard(bot_move);

            if ((winner = CheckWinner()) != NONE)
            {
                emit EndGame(winner);
                return;
            }
        }
    }
}

std::vector<std::vector<player>>& Model::GetBoard()
{
	return board;
}

std::pair<short, short> Model::MakeBotMove()
{
	srand(time(NULL));
	
	short x = rand() % field_size;
	short y = rand() % field_size;

	while (board[x][y] != NONE)
	{
		x = rand() % field_size;
		y = rand() % field_size; 
	}

    board[x][y] = cur_player;

    return std::make_pair(x, y);
}

player Model::CheckWinner()
{
	for (auto& it_1 : board)
	{
		for (auto& it_2 : it_1)
		{
			if (it_2 == NONE)
			{
				goto exit;
			}
		}
	}

	return DRAW;

	exit:

	player winner;

	if ((winner = CheckDiagonal()) != NONE)
	{
		return winner;
	}
	else if ((winner = CheckHorizontal()) != NONE)
	{
		return winner;
	}
	else if ((winner = CheckVertical()) != NONE)
	{
		return winner;
	}

	return NONE;
}

player Model::CheckHorizontal()
{
	for (int i = 0; i < field_size; ++i)
	{
		int count = 0;
		
		for (int j = 0; j < field_size-1; ++j)
		{
			if (board[i][j] == NONE)
			{
				break;
			}
			
			if (board[i][j] == board[i][j + 1])
			{
				++count;
			}
			else
			{
				break;
			}
		}

		if (count == field_size -1)
		{
			return board[i][0];
		}
	}

	return NONE;
}

player Model::CheckVertical()
{
	for (int i = 0; i < field_size; ++i)
	{
		int count = 0;
		
		for (int j = 0; j < field_size-1; ++j)
		{
			if (board[j][i] == NONE)
			{
				break;
			}
			
			if (board[j][i] == board[j + 1][i])
			{
				++count;
			}
			else
			{
				break;
			}
		}

		if (count == field_size -1)
		{
			return board[i][0];
		}
	}

	return NONE;
}

player Model::CheckDiagonal()
{
	int count = 0;
	player winner = NONE;
	for (int i = 0; i < field_size; i += field_size - 1)
	{
		for (int j = 0, k = i; j < field_size - 1; ++j, i == 0 ? ++k : --k)
		{
			if (board[j][k] == NONE)
			{
				break;
			}
			
			if (board[j][k] == board[j + 1][i == 0 ? k + 1 : k - 1])
			{
				++count;
				winner = board[j][k];
			}
			else
			{
				break;
			}
		}
	}

	if (count == field_size -1)
	{
		return winner;
	}
	else
	{
		return NONE;
    }
}

void Model::ChangePlayer()
{
   cur_player = static_cast<player>((static_cast<int>(cur_player) + 1) % 2);
}


