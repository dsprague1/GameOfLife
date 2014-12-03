#include "library.h"
#include "soundlib.h"

const int nWindowLength = 500;
const int nWindowWidth = 500;
const double pi = acos(-1);

struct noteButton
{
	double startTime;
	Sound note;
	bool on;
};

struct gameBoard
{
	int mLength, mWidth;
	int cell_length, cell_width;
	noteButton occupied[25][25];
	bool secondary[25][25];
};

void draw_grid(gameBoard &gB)
{
	gB.cell_length = nWindowLength/gB.mLength; 
	gB.cell_width = nWindowWidth/gB.mWidth;
	set_pen_color(0.5, 0.5, 0.5);
	set_pen_width(1);
	
	for(int i = gB.cell_length; i < nWindowLength; i+=gB.cell_length)
	{
		move_to(i, nWindowLength);
		draw_to(i, 0);
	}

	for(int j = gB.cell_width; j < nWindowWidth; j+=gB.cell_width)
	{
		move_to(nWindowWidth, j);
		draw_to(0, j);
	}
}

gameBoard findlw(string userinput)
{
	gameBoard gB;
	for(int i = 0; i < userinput.size(); i++)
	{	
		if(userinput[i] == 'X' || userinput[i] == 'x')
		{
			gB.mLength = stod(userinput.substr(0, i));
			gB.mWidth = stod(userinput.substr(i+1, userinput.size()-(i+1)));
		}
	}
	return gB;
}

int neighbors(noteButton occupied[][25], int curr_x, int curr_y)
{
	 int count = 0;
	 if(curr_x > 0 && curr_y > 0)//condition for upper left square
		 if(occupied[curr_x-1][curr_y-1].on)
			 count++;
	 if(curr_x > 0)//condition for left square
		 if(occupied[curr_x-1][curr_y].on)
			 count++;
	 if(curr_x > 0 && curr_y < 24)//bottom left square
		 if(occupied[curr_x-1][curr_y+1].on)
			 count++;
	 if(curr_y < 24)//bottom middle square
		 if(occupied[curr_x][curr_y+1].on)
			count++;
	 if(curr_x < 24 && curr_y < 24)//bottom right square
		 if(occupied[curr_x+1][curr_y+1].on)
			 count++;
	 if(curr_x < 24)//condition for right square
		 if(occupied[curr_x+1][curr_y].on)
			 count++;
	 if(curr_x < 24 && curr_y > 0)//condition for upper right square
		 if(occupied[curr_x+1][curr_y-1].on)
			 count++;
	 if(curr_y > 0)//top middle square
		 if(occupied[curr_x][curr_y-1].on)
			count++;
	 
	 return count;
}

void stop(noteButton curr)
{
	double now = read_timer();
	double timeLeft = now - curr.startTime;
	int fadeLength = (timeLeft)*44100;
	double f = 1.0;
	for(int i = fadeLength; i > 0; i--)
	{
		curr.note[i] *= f;
		f -= 0.001;
	}
}

void refresh(gameBoard &gB)
{
	for(int i = 0; i < 25; i++)
		for(int j = 0; j < 25; j++)
		{
			gB.occupied[j][i].on = gB.secondary[j][i];
			if(gB.occupied[j][i].on)
				gB.occupied[j][i].note.play();
			else
				stop(gB.occupied[j][i]  );
		}
}

void LIFE(gameBoard myGameBoard)
{
	while(true)
	{
		for(int i = 0; i < 25; i++)
		{
			for(int j = 0; j < 25; j++)
			{
				int num_neighbors = neighbors(myGameBoard.occupied, j, i);
				/******
					Rule 1: 
					Any live cell with fewer than two live neighbors 
					dies, as if caused by under-population or 
					lonliness.
															******/
				if(num_neighbors < 2 && myGameBoard.occupied[j][i].on == true)
				{
					myGameBoard.secondary[j][i] = false;
					set_pen_color(1.0,1.0,1.0);
					fill_rectangle(j*myGameBoard.cell_width+1,i*myGameBoard.cell_length+1, 
						myGameBoard.cell_length-1, myGameBoard.cell_width-1);
					continue;
				}
				/******
					Rule 2: 
					Any live cell with two or three live neighbors
					lives on to the next generation.
															******/
				if((num_neighbors == 2 || num_neighbors == 3) && myGameBoard.occupied[j][i].on == true)
					continue;
				
				/******
					Rule 3: 
					Any live cell with more than three live neighbors
					dies, as if by overcrowding.
														   ******/	
				if(num_neighbors > 3 && myGameBoard.occupied[j][i].on == true)
				{
					myGameBoard.secondary[j][i] = false;
					set_pen_color(1.0,1.0,1.0);
					fill_rectangle(j*myGameBoard.cell_width+1,i*myGameBoard.cell_length+1, 
						myGameBoard.cell_length-1, myGameBoard.cell_width-1);
					continue;
				}
				/******
					Rule 4: 
					Any dead cell with exactly three live neighbors
					becomes a live cell, as if by reproduction.
															******/
				if(num_neighbors == 3 && myGameBoard.occupied[j][i].on == false)
				{
					myGameBoard.secondary[j][i] = true;
					set_pen_color(1.0,1.0,0.0);
					fill_rectangle(j*myGameBoard.cell_width+1,i*myGameBoard.cell_length+1, 
						myGameBoard.cell_length-1, myGameBoard.cell_width-1);
					continue;
				}
				//cin.get();
			}
		}
		char pause = wait_for_key_typed(3);
		if(pause == 32)
			return;
		refresh(myGameBoard);
	}
}

void game_loop(gameBoard myGameBoard)
{
	set_pen_color(1.0, 1.0, 0.0);
	int x_pos, y_pos;
	int cell_num_width, cell_num_length;
	char which_one;
	while(true)
	{
		which_one = wait_for_key_typed_or_mouse_click();
		if(which_one == 0) //mouse click
		{
			x_pos = get_click_x();
			y_pos = get_click_y();
			cell_num_width = x_pos/myGameBoard.cell_width;
			cell_num_length = y_pos/myGameBoard.cell_length;

			if(!myGameBoard.occupied[cell_num_width][cell_num_length].on)
			{
				set_pen_color(1.0, 1.0, 0.0);
				myGameBoard.occupied[cell_num_width][cell_num_length].on = true;
				myGameBoard.secondary[cell_num_width][cell_num_length] = true;
				fill_rectangle(cell_num_width*myGameBoard.cell_width+1,cell_num_length*myGameBoard.cell_length+1, 
					myGameBoard.cell_length-1, myGameBoard.cell_width-1);
			}
			else if(myGameBoard.occupied[cell_num_width][cell_num_length].on)
			{
				set_pen_color(1.0,1.0,1.0);
				myGameBoard.occupied[cell_num_width][cell_num_length].on = false;
				myGameBoard.secondary[cell_num_width][cell_num_length] = false;
				fill_rectangle(cell_num_width*myGameBoard.cell_width+1,cell_num_length*myGameBoard.cell_length+1, 
					myGameBoard.cell_length-1, myGameBoard.cell_width-1);
			}
		}
		else if(which_one == 32) //spacebar
			LIFE(myGameBoard);
		else if(which_one == 'q')
			exit(1);
	}
}

Sound makeNote(double frequency)
{
	Sound a(4.0, 44100);
	for(int i = 0; i < 44100*4; i++)
		a[i] = (int)(32767.0*sin(2*pi*(double)(i/44100.0)*frequency));
	return a;
}

void main()
{
	gameBoard myGameBoard;
	string userinput;
	Sound a = makeNote(440);
	//cout << "How large should Game of Life grid be(lXw)?\n";
	//cin >> userinput;
	//myGameBoard = findlw(userinput);
	myGameBoard.mLength = 25;
	myGameBoard.mWidth = 25;
	for(int i = 0; i < 25; i++)
		for(int j = 0; j < 25; j++)
		{
			myGameBoard.occupied[j][i].on = false;
			//myGameBoard.occupied[j][i].note = makeNote(440.0);
			myGameBoard.secondary[j][i] = false;
		}
	make_window(nWindowWidth, nWindowLength);
	draw_grid(myGameBoard);
	game_loop(myGameBoard);
}