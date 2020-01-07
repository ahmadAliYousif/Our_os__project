#include "kernel.h"
#include "utils.h"
#include "char.h"

uint32 vga_index;
uint16 cursor_pos = 0, cursor_next_line_index = 1;
static uint32 next_line_index = 1;
uint8 g_fore_color = WHITE, g_back_color = BLACK;

// if running on VirtualBox, VMware or on raw machine, 
// change CALC_SLEEP following to greater than 4
// for qemu it is better for 1
#define CALC_SLEEP 1
static char square[10] = { 'o', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
/*
this is same as we did in our assembly code for vga_print_char

vga_print_char:
  mov di, word[VGA_INDEX]
  mov al, byte[VGA_CHAR]

  mov ah, byte[VGA_BACK_COLOR]
  sal ah, 4
  or ah, byte[VGA_FORE_COLOR]

  mov [es:di], ax

  ret

*/
uint16 vga_entry(unsigned char ch, uint8 fore_color, uint8 back_color) 
{
  uint16 ax = 0;
  uint8 ah = 0, al = 0;

  ah = back_color;
  ah <<= 4;
  ah |= fore_color;
  ax = ah;
  ax <<= 8;
  al = ch;
  ax |= al;

  return ax;
}

void clear_vga_buffer(uint16 **buffer, uint8 fore_color, uint8 back_color)
{
  uint32 i;
  for(i = 0; i < BUFSIZE; i++){
    (*buffer)[i] = vga_entry(NULL, fore_color, back_color);
  }
  next_line_index = 1;
  vga_index = 0;
}

void clear_screen()
{
  clear_vga_buffer(&vga_buffer, g_fore_color, g_back_color);
  cursor_pos = 0;
  cursor_next_line_index = 1;
}

void init_vga(uint8 fore_color, uint8 back_color)
{
  vga_buffer = (uint16*)VGA_ADDRESS;
  clear_vga_buffer(&vga_buffer, fore_color, back_color);
  g_fore_color = fore_color;
  g_back_color = back_color;
}

uint8 inb(uint16 port)
{
  uint8 data;
  asm volatile("inb %1, %0" : "=a"(data) : "Nd"(port));
  return data;
}

void outb(uint16 port, uint8 data)
{
  asm volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}

void move_cursor(uint16 pos)
{
  outb(0x3D4, 14);
  outb(0x3D5, ((pos >> 8) & 0x00FF));
  outb(0x3D4, 15);
  outb(0x3D5, pos & 0x00FF);
}

void move_cursor_next_line()
{
  cursor_pos = 80 * cursor_next_line_index;
  cursor_next_line_index++;
  move_cursor(cursor_pos);
}

void gotoxy(uint16 x, uint16 y)
{
  vga_index = 80*y;
  vga_index += x;
  if(y > 0){
    cursor_pos = 80 * cursor_next_line_index * y;
    cursor_next_line_index++;
    move_cursor(cursor_pos);
  }
}

char get_input_keycode()
{  
  char ch = 0;
  while((ch = inb(KEYBOARD_PORT)) != 0){
    if(ch > 0)
      return ch;
  }
  return ch;
}

/*
keep the cpu busy for doing nothing(nop)
so that io port will not be processed by cpu
here timer can also be used, but lets do this in looping counter
*/
void wait_for_io(uint32 timer_count)
{
  while(1){
    asm volatile("nop");
    timer_count--;
    if(timer_count <= 0)
      break;
    }
}

void sleep(uint32 timer_count)
{
  wait_for_io(timer_count*0x02FFFFFF);
}

void print_new_line()
{
  if(next_line_index >= 55){
    next_line_index = 0;
    clear_vga_buffer(&vga_buffer, g_fore_color, g_back_color);
  }
  vga_index = 80*next_line_index;
  next_line_index++;
  move_cursor_next_line();
}

void print_char(char ch)
{
  vga_buffer[vga_index] = vga_entry(ch, g_fore_color, g_back_color);
  vga_index++;
  move_cursor(++cursor_pos);
}

void print_string(char *str)
{
  uint32 index = 0;
  while(str[index]){
    if(str[index] == '\n'){
      print_new_line();
      index++;
    }else{
      print_char(str[index]);
      index++;
    }
  }
}

void print_int(int num)
{
  char str_num[digit_count(num)+1];
  itoa(num, str_num);
  print_string(str_num);  sleep(CALC_SLEEP);  sleep(CALC_SLEEP);  sleep(CALC_SLEEP);
}

int read_int()
{

  char ch = 0;
  char keycode = 0;
  char data[32];
  int index = 0;
    sleep(CALC_SLEEP);  sleep(CALC_SLEEP);  sleep(CALC_SLEEP);  sleep(CALC_SLEEP);
  do{
    keycode = get_input_keycode();
	
    if(keycode == KEY_ENTER){
      data[index] = '\0';
      print_new_line();
      break;
    }else{
      ch = get_ascii_char(keycode);
      print_char(ch);
      data[index] = ch;
      index++;
    }
  sleep(CALC_SLEEP);
  sleep(CALC_SLEEP);
  sleep(CALC_SLEEP);  sleep(CALC_SLEEP);
 
  }while(ch > 0);

  return atoi(data);
}

char getchar()
{
  char keycode = 0;
  sleep(CALC_SLEEP);
  keycode = get_input_keycode();
  sleep(CALC_SLEEP);
  return get_ascii_char(keycode);
}
int log_in(int id,int pass){
int username=123456;
int password=123456;



 
if(username==id&&password==pass){clear_screen();
  print_string("\n\n\n\n\n                $$  wellocme DR.hazem al bazz to our os  $$         ");
 
 sleep(5700);

return 1;

}else{
  print_string("\n error username or password");

return 0;
}


}
void display_menu()
{
  gotoxy(25, 0);
  print_string("\n\n                               WELLCOME   TO      ");
  print_string("\n\n                                  Calculator     ");
  print_string("\n\n!------------------------ Mathematical opertations -------------------!");
  print_string("\n1- SUM     ");
  print_string("\n2- SUB     ");
  print_string("\n3- Mult    ");
  print_string("\n4- Div     ");
  print_string("\n5- Fact    ");
  print_string("\n6- Exit    ");

}

void read_two_numbers(int* num1, int* num2)
{
  print_string("\n Enter first number : ");
  sleep(CALC_SLEEP);  sleep(CALC_SLEEP);  sleep(CALC_SLEEP);
  *num1 = read_int();
  print_string("\n Enter second number : ");
  sleep(CALC_SLEEP);  sleep(CALC_SLEEP);  sleep(CALC_SLEEP);
  *num2 = read_int();
}

void read_one_numbers(int* num1)
{
  print_string("\n Enter number : ");
  sleep(CALC_SLEEP);  sleep(CALC_SLEEP);  sleep(CALC_SLEEP);
  *num1 = read_int();
 
}

int calcu()
{
  int choice, num1, num2,t=1;
  while(t==1){
//n1
    display_menu();
    print_string("\n\nEnter your choice : ");
    choice = read_int();
    switch(choice){
      case 1:
        read_two_numbers(&num1,& num2);
        print_string(" Addition : ");
        print_string(" \nthe rsult : ");
        print_int(num1 + num2);
	print_new_line();
    	print_string("\n Press any key to reload screen...");
   	 getchar();	
	
   	 clear_screen();
	 
        break;
      case 2:
        read_two_numbers(&num1, &num2);
        print_string(" Substraction : ");        print_string(" \nthe rsult : ");
        print_int(num1 - num2);

	print_new_line();
    	print_string("\n Press any key to reload screen...");
   	 getchar();	
	
   	 clear_screen();   	   
 break;
      case 3:
        read_two_numbers(&num1,&num2);
        print_string(" Multiplication : ");        print_string(" \nthe rsult : ");
        print_int(num1 * num2);	print_new_line();
    	print_string("\n Press any key to reload screen...");
   	 getchar();	
	
   	 clear_screen();	 
        break;
      case 4:
        read_two_numbers(&num1, &num2);
        if(num2 == 0){
	  print_new_line();
          print_string("Error: Divide by 0");
        }else{
          print_string("Division : ");        print_string(" \nthe rsult : ");
          print_int(num1 / num2);
        }
	print_new_line();
    	print_string("\n Press any key to reload screen...");
   	 getchar();	
	
   	 clear_screen();	 
        break;
      case 5 :
            read_one_numbers(&num1);
	        int fact=1;
		for(int i=1;i<=num1;i++){
		fact=fact*i;

}                print_string(" \nthe rsult : ");  print_int(fact);	print_new_line();
    	print_string("\n Press any key to reload screen...");
   	 getchar();	
	
   	 clear_screen();

	 
	break;
      case 6:
clear_screen();
init_vga(BROWN, WHITE);
	 print_new_line();
         print_string("\n\n\n\n\n\n\n           ********************!!! goodBay ^_^ !!!**********************");
 	t=2;
	 return 1;
         break;	
	
	
      default:
	print_new_line();
        print_string(" Invalid choice...!");	print_new_line();
    	print_string("\n Press any key to reload screen...");
   	 getchar();	
	
   	 clear_screen();

	 
        break;
    }
	
	
  }
}


void tree() {

int rows = 12;
for (int i = 1; i <= rows; i=i+1) { 
print_string("                           ");
 sleep(2);  
for (int x = 0; x<rows - i; x=x+1)print_string(" ");
for (int x =0 ; x<(i* 2 )- 1; x=x+1) print_string("*");  sleep(CALC_SLEEP);
	 print_new_line();
}
  
for (int i = rows/2 + 1; i <= rows; i=i+1) {
print_string("                           ");
for (int x = 0; x<rows - i; x=x+1) print_string(" "); 
for (int x = 0; x<(i * 2 )- 1; x=x+1)  print_string("*");sleep(CALC_SLEEP);

	 print_new_line();

}


print_string("                           ");

for (int i = 0; i <rows - 2; i=i+1) print_string(" ");
print_string("|=|\n");
print_string("                           ");
for (int i = 0; i <rows - 2; i=i+1) print_string(" ");
print_string("|=|\n");

print_string("   <<<<<<<<<<<<      -_-     happy   ");sleep(2); print_string("new   ");  sleep(2);print_string("Year   ");sleep(2);  print_string("     -_-  >>>>>>>>>");	 print_new_line() ;print_new_line() ;print_new_line() ;sleep(CALC_SLEEP);  sleep(CALC_SLEEP);sleep(2);  
print_string(" **********                        2  ");  sleep(2);print_string("0  "); sleep(2);print_string("2  ");  sleep(1);print_string("0                         *****");
 }


 
int tic_tot()
{
    int player = 1, i, choice;

    char mark;
    do
    {
        board();

        player = (player % 2) ? 1 : 2;

        print_string("Player : " );
	print_int(player); 
	print_string(" enter a number:"); 
 
     choice=   read_int();

        mark = (player == 1) ? 'X' : 'O';
        if (choice == 1 && square[1] == '1')
            square[1] = mark;
            
        else if (choice == 2 && square[2] == '2')
            square[2] = mark;
            
        else if (choice == 3 && square[3] == '3')
            square[3] = mark;
            
        else if (choice == 4 && square[4] == '4')
            square[4] = mark;
            
        else if (choice == 5 && square[5] == '5')
            square[5] = mark;
            
        else if (choice == 6 && square[6] == '6')
            square[6] = mark;
            
        else if (choice == 7 && square[7] == '7')
            square[7] = mark;
            
        else if (choice == 8 && square[8] == '8')
            square[8] = mark;
            
        else if (choice == 9 && square[9] == '9')
            square[9] = mark;
            
        else
        {
            print_string("Invalid move ");

            player--;
   	 getchar();	
        }
        i = checkwin();

        player=player+1;
    }while (i ==   2);
    
    board();
    
    if (i == 1){ 
        print_string("==>Player    ");
	print_int(player-1);        print_string("  win ");
	
	print_string(player--);
}
    else
        print_string("==>Game draw");

   	 getchar();	

    return 0;
}

/*********************************************

FUNCTION TO RETURN GAME STATUS
1 FOR GAME IS OVER WITH RESULT
-1 FOR GAME IS IN PROGRESS
O GAME IS OVER AND NO RESULT
 **********************************************/

int checkwin()
{
    if (square[1] == square[2] && square[2] == square[3])
        return 1;
        
    else if (square[4] == square[5] && square[5] == square[6])
        return 1;
        
    else if (square[7] == square[8] && square[8] == square[9])
        return 1;
        
    else if (square[1] == square[4] && square[4] == square[7])
        return 1;
        
    else if (square[2] == square[5] && square[5] == square[8])
        return 1;
        
    else if (square[3] == square[6] && square[6] == square[9])
        return 1;
        
    else if (square[1] == square[5] && square[5] == square[9])
        return 1;
        
    else if (square[3] == square[5] && square[5] == square[7])
        return 1;
        
    else if (square[1] != '1' && square[2] != '2' && square[3] != '3' &&
        square[4] != '4' && square[5] != '5' && square[6] != '6' && square[7] 
        != '7' && square[8] != '8' && square[9] != '9')

        return 0;
    else
        return  2;
}


/*******************************************************************
FUNCTION TO DRAW BOARD OF TIC TAC TOE WITH PLAYERS MARK
 ********************************************************************/


void board()
{
clear_screen();
    print_string("\n\n\tTic Tac Toe\n\n");

    print_string("Player 1 (X)  -  Player 2 (O)\n\n\n");


    print_string("     |     |     \n");
   print_string("   ");print_char( square[1]); print_string(" |  ");print_char( square[2]);print_string("  |  ");print_char( square[3]);print_string("  \n");

    print_string("_____|_____|_____\n");
    print_string("     |     |     \n");

 print_string("   ");  print_char( square[4]); print_string(" |  ");print_char( square[5]);print_string("  |  ");print_char( square[6]);print_string("  \n");

    print_string("_____|_____|_____\n");
    print_string("     |     |     \n");

 print_string("   ");  print_char( square[7]); print_string(" |  ");print_char( square[8]);print_string("  |  ");print_char( square[9]);print_string("  \n");

    print_string("     |     |     \n\n");
 

 


 
}

 


void kernel_entry()
{ 
  init_vga(BLUE, CYAN);
  sleep(CALC_SLEEP);  sleep(CALC_SLEEP);
print_string("\n Enter your id 1: ");
	int name=read_int();
  sleep(CALC_SLEEP);  sleep(CALC_SLEEP);
  print_string("\n Enter password : ");	
	int pass=read_int();
	int s=log_in(name,pass);
	int f=0;int c;
	while(s==1||f<2){
	
	if(s==1){
   init_vga(MAGENTA, GREY);
  print_string("\n choose one : calcolater (1) , tic_toc(2) ,  exit(3) : ");	
	c=read_int();
	switch(c){
case 1:
calcu();
break;
case 2:
tic_tot();
break;
case 3 :
 print_string("\n good bay ....... ");

sleep(10);
clear_screen();
tree();
return 0;
	
      default:
	print_new_line();
        print_string(" Invalid choice...!");	print_new_line();
    	print_string("\n Press any key to reload screen...");
   	 getchar();	
	
   	 clear_screen();

	 
        break;


}
	

	f=4;
}
	else{	print_new_line();
	 print_string("\ntry agin...!  ");
   print_new_line();

print_string("\n Enter your id : ");print_new_line();
	name=read_int();  sleep(CALC_SLEEP);  sleep(CALC_SLEEP);
  print_string("\n Enter password : ");
	pass=read_int();  sleep(CALC_SLEEP);  sleep(CALC_SLEEP);
	s=log_in(name,pass);
	f=f+1;
	if(f>1){ 
 print_string("\ngo way");
}
	

}
	}

 


sleep(10);
clear_screen();


}

