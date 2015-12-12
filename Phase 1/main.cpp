/*************************************
* Supervisor
* Supervisor program for embedded controller
* Communicates with controller via serial
* Loren Copeland
* April 21, 2015
* Reference: http://en.wikibooks.org/wiki/Serial_Programming/termios
**************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <stdint.h>
#include <linux/joystick.h>
#include <sys/wait.h>
#include <math.h>
#include "video.h"
#include <thread>

//macros
#define DEVICE_MAX_SIZE 50
#define BAUD_RATE B9600

#define MENUFILE "menu.txt"
#define MENU_MAX_LINES 80
#define MENU_MAX_LINE_LENGTH 80

#define CMD_LENGTH 6
#define PING_INTERVAL 1
#define PING_RETRIES 5
#define NO_RESPONSE -1

#define AUDIO_FILENAME "capture.wav"

#define LEFT 1
#define RIGHT 2
#define IMAGE "image.jpeg"
#define SIZE 307200 // number of pixels

//structures
typedef struct packet {
    unsigned char cmd;
    unsigned char arg1;
    unsigned char arg2;
} packet;

//prototypes
int captureAudio(char *fileName);
int configDevice(void);
int checkDTMF(char *fileName);
void menu(void);
char parseCommand(unsigned char cmd, unsigned char arg1, unsigned char arg2);
char send(unsigned char cmd, unsigned char arg1, unsigned char arg2);
char receive(packet *recievepkt);
int video_process();

int open_joystick();
int button_press(int button);
int get_angle(int value);
int axis_press(int axis, int value);
int move_servo(int value);

unsigned char servo_position = 128;


//globals
static char device[DEVICE_MAX_SIZE];
static struct termios settings;
std::thread t1;
bool image_quit = false;
bool image_capture = false;


//move this back into main after debugging
static int fd;
unsigned int timeouts = 0;

//child (ping loop) process id
pid_t pid, pid2;

//Signal Handlers

/*************sendPing*****************
* Sends ping when alarm sounds
*****************************************/
void sendPing(int signal_number){
    if(parseCommand('P', 0, 0) == NO_RESPONSE){
        timeouts++;
        printf("Warning: Ping timeout %d\n", timeouts);
        }
    alarm(PING_INTERVAL);
    signal(SIGALRM, sendPing);
}

/*************catchTimeoutReset**********
* Handle timeout counter reset signal
****************************************/
void catchTimeoutReset(int signal_number){
    timeouts = 0;
    alarm(PING_INTERVAL);
    signal(SIGUSR1, catchTimeoutReset);
}

/*************catchClose****************
* Handle close signal
*****************************************/
void catchClose(int signal_number){
    printf("Exiting...");
    close(fd);
    kill(getpid(), SIGKILL);
}

int main(int argc, char *argv[]){
    unsigned char stop, temp;
    unsigned char cmd;
    unsigned char arg1, arg2;
    char arg1buf[4], arg2buf[4];
    packet recievepkt;
    packet sendpkt;


    int fd_joy;			// File Descriptor for joystick
	int temp1;
	struct js_event e;
	temp1 = 0;               // Set first time stamp to zero
	fd_joy = open_joystick();   // /Initalize the joystick
   // printf("fd_joy: %d\n", fd_joy);


    strncpy(device, argv[1], DEVICE_MAX_SIZE);

    //set up serial port
    if((fd = configDevice()) < 2){
        perror("Error opening/configuring device\n");
        return -1;
        }

    menu();

    //set handler for close command
    signal(SIGUSR2, catchClose);
    signal(SIGTERM, catchClose);

    //create timer for ping
    pid = fork();
    if (pid < 0) printf("Error creating ping timer process\n");
    else if(pid == 0){
        //die if parent dies
        prctl(PR_SET_PDEATHSIG, SIGHUP);
        //set alarm for ping timeout
        signal(SIGALRM, sendPing);
        //prime Timeout reset handler
        signal(SIGUSR1, catchTimeoutReset);

        alarm(PING_INTERVAL);

        for(timeouts = 0; timeouts < PING_RETRIES;){
            //spin wheels. exit if ping timeout occurs
        }
        //kill parent process if ping timeout
        printf("Ping timeout. %d unanswered pings\n", timeouts);
        //tell parent to exit
        kill(getppid(), SIGKILL);
        return 1;
    }

    int readcount;
    char zeroflag;


    //main control loop
    for(stop = 0; stop <= 0;){

        //cmd = 'V';
        //arg1 = 0;
        //arg2 = 0;
        /*scanf("%c %s %s%c", &cmd, arg1buf, arg2buf, NULL);        //get command
        //strcpy(arg1buf, "Z");
        arg1 = (unsigned char)atoi(arg1buf);
        arg2 = (unsigned char)atoi(arg2buf);

        if(arg1 == 0){
            arg1 = 1;
        }
        if(arg2 == 0){
            arg2 = 1;
        }*/

        readcount = read (fd_joy, &e, sizeof(e));
		//printf("servo: %d\r", e.time);
        //printf("bytes read from joystick: %d\n", readcount);
		//if(e.time > temp1)
		{
            //printf("Bryan\n");
			if(e.type == 1 && e.value == 1){
                if(e.time > temp1){
                    button_press(e.number);
                    temp1 = e.time;
                    }
				}
			if(e.type == 2 /*&& e.value != 0*/){
			if(!zeroflag){
				axis_press(e.number, e.value);
				}
			//set zeroflag if event input zero
			//necessary to not keep resending packages
			//when in the zero (stopped) position
            zeroflag = (e.value == 0) ? 1 : 0;
            //printf("zeroflag %c\n", zeroflag);
            }
		}

        //while((temp = getchar()) != '\n' && temp != EOF);  //clear buffer
        //printf("CMD: %c ARG1: %c ARG2: %c\n", cmd, arg1, arg2);
        //stop = parseCommand(cmd, arg1, arg2);
    }
    close(fd);
    //tell child to exit
    kill(pid, SIGUSR2);
    t1.join();
    scanf("", NULL);
    return 0;
}

/*************configDevice***************
* Configure serial device for raw data
* Output: fd - file descriptor of serial device
            -1 for error
*****************************************/
int configDevice(void){
    int fd;
    char script[200];
    //flags: Don't make opened device controlling terminal
    // read/write
    // non-blocking
    if((fd = open((const char *)device, O_NOCTTY | O_RDWR | O_NONBLOCK)) < 2){
        perror("Error opening serial device\n");
        return -1;
        }

    //verify device type
    if(!isatty(fd)){
        perror("Device is not a tty\n");
        return -1;
        }

    //copy current serial configuration
    if(tcgetattr(fd, &settings) < 0){
        perror("Could not get device attributes\n");
        return -1;
        }

    //input flags
    settings.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);

    //output flags -- kill all output processing
    settings.c_oflag = 0;

    //turn off line processing
    settings.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);

    //turn off character processing
    settings.c_cflag &= ~(CSIZE | PARENB);
    settings.c_cflag |= CS8;

    //read one character without a timer
    settings.c_cc[VMIN]  = 1;
    settings.c_cc[VTIME] = 0;

    //set baud rate
    if(cfsetispeed(&settings, BAUD_RATE) < 0){
        perror("Could not set input baud rate\n");
        return -1;
        }
    if(cfsetospeed(&settings, BAUD_RATE) <0){
        perror("Could not set output baud rate\n");
        return -1;
        }

        //build string to run script to actually fix tty settings
        strcpy(script, "./make-tty-work.sh ");
        strcat(script, device);

        puts(script);
        system(script);

    return fd;
}

/*****************menu*****************
* Displays program menu
***************************************/
void menu(){
    FILE *menu;
    int line;
    char buf[MENU_MAX_LINES*MENU_MAX_LINE_LENGTH];

    //Grab menu from file
    if ((menu = fopen(MENUFILE, "r")) == NULL){
        printf("Error: Cannot read menu file.\n");
        return;
    }

    printf("\n*****************************************\n");
    printf("Embedded Controller Supervisor Program\n");
    printf("*****************************************\n");
    //Read and print each menu line
    for(line = 0;
        line < MENU_MAX_LINES && (fgets(buf, MENU_MAX_LINE_LENGTH, menu) != NULL);
        line++){
        printf("%s", buf);
    }
    return;
}

/****************parseCommand*************
* Decides what to do based on user input
* Output: 0 - stay in program
          anything else - exit
******************************************/
char parseCommand(unsigned char cmd, unsigned char arg1, unsigned char arg2){
    packet pkt;
    char arg1buf[4], arg2buf[4];
    char retval = 0;
    switch(cmd){
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'F':
        case 'G':
        case 'H':
        case 'K':
        case 'L':
        case 'O':
        case 'P':
            send(cmd, arg1, arg2);
            usleep(6000);
            retval = receive(&pkt);
            sprintf(arg1buf, "%d", arg1);
            sprintf(arg2buf, "%d", arg2);
            printf("cmd: %c arg1: %s arg2: %s\n", pkt.cmd, arg1buf, arg2buf);
            break;
        case 'R':
            retval = receive(&pkt);
            //printf("cmd: %c arg1: %c arg2: %c\n", pkt.cmd, pkt.arg1, pkt.arg2);
            sprintf(arg1buf, "%d", arg1);
            sprintf(arg2buf, "%d", arg2);
            printf("cmd: %c arg1: %s arg2: %s\n", pkt.cmd, arg1buf, arg2buf);
            break;
        case 'S':
            printf("Recording audio...");
            system("./get-dtmf.sh");
            break;
        case 'Z':
            menu();
            break;
        case 'X':
            return 1;
            break;
        default:
            printf("Invalid command\n");
            break;
    }
    return retval;
}


/*************send*************
* Sends a command to controller over serial line
* Input: cmd - command
         arg1 - first argument
         arg2 - second argument
* Output: 0 if write successful
**************************************/
char send(unsigned char cmd, unsigned char arg1, unsigned char arg2){
    char buf[CMD_LENGTH];
    sprintf(buf, "%c %c %c%c", cmd, arg1, arg2, 13);
    if(write(fd, buf, CMD_LENGTH) != CMD_LENGTH){
        perror("Command write failed\n");
        return -1;
        }
    return 0;
}

/*************receive************
* Listens for a packet on serial line
* validates packet
* places packet into structure
* Input: receivepkt - buffer to place valid packet in
* Output: 0 on success
*********************************/
char receive(packet *receivepkt){
    char buf[CMD_LENGTH];

    if(read(fd, buf, CMD_LENGTH) < CMD_LENGTH){
        perror("Received: Packet read error\n");
        return NO_RESPONSE;
    }
    //got response from platform. Reset ping counter
    else {
        //if parent UI process, tell child to reset timeout counter
        if(pid > 0){
            kill(pid, SIGUSR1);
        }
        else{ //child process. reset timeout counter

        alarm(PING_INTERVAL);
        timeouts = 0;
        }
    }


    //printf("%s\n", buf);
    /*error checking */

    receivepkt->cmd = buf[0];
    receivepkt->arg1 = buf[2];
    receivepkt->arg2 = buf[4];
    return 0;
}

/*
Initalize the joystick
*/
int open_joystick()
{
	int fd_joy;

	fd_joy = open("/dev/input/js0", O_RDONLY | O_NONBLOCK); /* read write for force feedback? */

	if(fd_joy > 0)
		return fd_joy; // open was successful
	else
		return -5; // open failed
}

int axis_press(int axis, int value)
{

	switch(axis)
	{
        //turn platform
		case 0:
            //left turn
            if(value >= 3000 && value <= 32767){
                parseCommand('A', (char)((double)value*0.0006718 + 12.987), 9);
                }
            //right turn
            else if(value <= -3000 && value >= -32767){
                parseCommand('A', (char)((double)(abs(value))*0.0006718 + 12.987), 6);
                //parseCommand('A', '"', 6);
                }
            else{
                parseCommand('A', 0, 0);                //stop motor
                printf("stop\n");

            }
			printf("LH @ %d\n", value);
			break;
        //move motors forward / backward
		case 1:
            if(value >= 3000 && value <= 32767)
            if
                parseCommand('A', (char)((double)value*0.0006718 + 12.987), 10);          // dividing by 130 keeps 32767 within range of char
            else if(value < -3000 && value >= -32767)
                parseCommand('A', (char)((double)(abs(value))*0.0006718 + 12.987), 5);
            else
                parseCommand('A', 0, 0);                //stop motor
			printf("LV @ %d\n", value);
			break;
		// Move the stepper motor left and right
		case 2:
            if(value > 1000 && value < 16000)
                parseCommand('G', 1, 0);
            else if(value > 16000)
                parseCommand('G', 2, 0);
            else if(value < -1000 && value > -16000)
                parseCommand('G', -1, 0);
            else if(value < -16000)
                parseCommand('G', -2, 0);
			break;
		// Move servo up and down
		case 3:
			//printf("RV @ %d\n", value);
			move_servo(value);
            //printf("servo: %d\n", servo_position);
            //sprintf(position,"%c", servo_position);
			parseCommand('K', servo_position, 0);
			break;
		case 4:
			printf("DH @ %d\n", value);
			break;
		case 5:
			printf("DV @ %d\n", value);
			break;
	}

	return 0;
}

/*
Sends a command when a button is pressed
*/
int button_press(int button)
{
    int i;
	switch(button)
	{
		case 0:
			printf("button 0 was pressed\n");
			image_capture = true;
			break;
        case 1:
			printf("button 1 was pressed\n");
			//move in loop A
			for (i = 0; i < 3; i++){
                parseCommand('B', 26, 34);  //move for .77s
                sleep(7);
                usleep(25000);
                usleep(25000);
                usleep(20000);
                parseCommand('A', 0, 0);    //rest for 1.23s
                sleep(12);
                usleep(30000);
                }
			break;
		case 2:
            printf("button 2 was pressed\n");
            //pivot 180 degrees
            parseCommand('B', 0, 35);       //turn
            sleep(3);                       //wait 3s
            parseCommand('A', 0, 0);        //stop
			break;
		case 3:
			//move in loop B
            printf("button 3 was pressed\n");
			for (i = 0; i < 4; i++){
                parseCommand('B', 35, 25);  //move for 4s
                sleep(4);
                parseCommand('A', 0, 0);    //rest for 16s
                sleep(16);
            }

			break;
		// Image Capture
		case 4:
			//printf("button 4 was pressed\n");
			image_quit = false;
			t1 = std::thread(video_cap);
			break;
		// Audio Capture
		case 5:
			//printf("button 5 was pressed\n");
			parseCommand('S','0','0');
			break;
		case 6:
			printf("button 6 was pressed\n");
			image_quit = true;
			break;
		case 7:
			printf("button 7 was pressed\n");
			break;
		// Exit
		case 8:
			//printf("button 8 was pressed\n");
			parseCommand('X','0','0');
		break;
		// Initalize board
		case 9:
			//printf("button 9 was pressed\n");
			parseCommand('F','0','0');
		break;
	}

	return 0;
}

// Calculate the angle from the joystick value
// 0 - 255 not 0 - 90
int get_angle(int value)
{
	return value/128;
}

// Changes value of servo motor position
int move_servo(int value)
{

	if(value < -1000)
	{
		servo_position++;
	}

	else if (value > 1000)
	{
		servo_position--;
	}

	if(servo_position > 200)
		servo_position = 200;
	if(servo_position < 50)
		servo_position = 50;

	return 0;
}

