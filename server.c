#include <sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include "single_customer.h"
#include "joint_customer.h"


char cur_user[40];		//keep track of current user for joint account

void ViewJointAdmin(int client_socket,char username1[],char username2[]);
void ViewCustomerAdmin(int client_socket,char username[]);
void WelcomeMenu(int client_socket);
void CustomerMenu(int client_socket, struct Single_Customer db,struct flock lock,int fd);
void AdminMenu(int client_socket);
void DeleteAccount(int client_socket);
void DeleteJointAccount(int client_socket);
void JointAccountMenu(int client_socket, struct Joint_Customer db,struct flock,int );
void UpdateCustomerDetails(struct Single_Customer db_user);
void JointUpdateDetails(struct Joint_Customer db_user);
int JointAccountExists(char username1[],char username2[]);
int CustomerAccountExists(char username[]);
void NewJointAccount(int client_socket);
void Add_PreReq(int client_socket);
void JointAccountLogin(int client_socket);
void ViewCustomerDetails(int client_socket, struct Single_Customer db,struct flock lock,int fd);
void ViewJointDetails(int client_socket, struct Joint_Customer db,struct flock lock,int fd);
void JointDeposit(int client_socket, struct Joint_Customer db,struct flock lock,int fd);
void CustomerDeposit(int client_socket, struct Single_Customer db,struct flock lock,int fd);
void JointWithdraw(int client_socket, struct Joint_Customer db,struct flock lock, int fd);
void CustomerWithdraw(int client_socket, struct Single_Customer db,struct flock lock,int fd);
void JointPassChange(int client_socket, struct Joint_Customer db,struct flock lock);
void CustomerPassChange(int client_socket, struct Single_Customer db,struct flock lock,int fd);
void AdminLogin(int client_socket);
void CustomerLogin(int client_socket);
void AddAccount(char username[], char password[],int client_socket);

int main()
{
	struct sockaddr_in server, client;			//one socket for listening from client, one socket for communication to client
	int sockfd, client_size, client_socket;
	sockfd = socket(AF_INET, SOCK_STREAM,0);	//IPv4, reliable stream (tcp)	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;		//specify host interface
	server.sin_port = htons(49994);				//specify port number
	if(bind(sockfd,(struct sockaddr *)(&server), sizeof(server))<0)				//bind the socket to server
	{
		perror("");
	}
	listen(sockfd,5);					//listen for 5 clients maximum
	client_size = sizeof(client);
	//communicate with client
	while(1)
	{
		client_socket = accept(sockfd, (struct sockaddr *)&client, &client_size);		//client socket
		if(fork() == 0)
		{
			close(sockfd);								//close parent process and listen for children only.
			WelcomeMenu(client_socket);					
			exit(0);
		}
		else
		{
			close(client_socket);
		}
	}	
	close(client_socket);
	close(sockfd);
	return 0;
}

void WelcomeMenu(int client_socket)
{
	int choice,fd;
	read(client_socket, &choice, sizeof(choice));
	switch(choice)
	{
		case 1: CustomerLogin(client_socket);
				break;
		case 2: AdminLogin(client_socket);
				break;
		case 3: JointAccountLogin(client_socket);
				break;
		case 4: exit(0);
				break;
	}
}

void CustomerMenu(int client_socket, struct Single_Customer db,struct flock lock,int fd)
{
	int choice;
	read(client_socket, &choice, sizeof(choice));
	switch(choice)
	{
		case 1: ViewCustomerDetails(client_socket,db,lock,fd);
				break;
		case 2: CustomerDeposit(client_socket,db,lock,fd);
				break;
		case 3: CustomerWithdraw(client_socket,db,lock,fd);
				break;
		case 4: CustomerPassChange(client_socket,db,lock,fd);
				break;
		case 5: lock.l_type=F_UNLCK;
				fcntl(fd,F_SETLK,&lock); 
				WelcomeMenu(client_socket);
				break;
	}
}

void AdminMenu(int client_socket)
{
	int choice;
	char username[40], password[40], username1[40], username2[40];
	read(client_socket, &choice, sizeof(choice));
	switch(choice)
	{
		case 1:
			Add_PreReq(client_socket);
			break;
		case 2:
			DeleteAccount(client_socket);
			break;
		case 3:
			read(client_socket, username, sizeof(username));
			if(CustomerAccountExists(username))
			{
				write(client_socket, "Account Found!\n", sizeof("Account Found!\n"));
				ViewCustomerAdmin(client_socket,username);
			}
			else
			{
				write(client_socket, "Account does not exist with the given Username!\n", sizeof("Account does not exist with the given Username!\n"));
			}
			AdminMenu(client_socket);
			break;
		case 4:
			NewJointAccount(client_socket);
			break;
		case 5:
			DeleteJointAccount(client_socket);
			break;
		case 6:
			read(client_socket, username1, sizeof(username1));
			read(client_socket, username2, sizeof(username2));
			if(JointAccountExists(username1,username2))
			{
				write(client_socket, "Account Found!\n", sizeof("Account Found!\n"));
				ViewJointAdmin(client_socket,username1,username2);
			}
			else
			{
				write(client_socket, "Joint Account does not exist with the given Username!\n", sizeof("Joint Account does not exist with the given Username!\n"));
			}
			AdminMenu(client_socket);
			break;
		default:
			WelcomeMenu(client_socket);				//else, go back to main menu on wrong input
	}
}

void Add_PreReq(int client_socket)						//called from admin functions, so flag = 0
{
	char username[40], password[40];
	read(client_socket, username, sizeof(username));		//read new user's username from client
	read(client_socket, password, sizeof(password));		//read new user's password from client

	if(!CustomerAccountExists(username))						//check if account exists or not
	{
		AddAccount(username, password, client_socket);
		AdminMenu(client_socket);
	}
	else
	{
		write(client_socket, "Username already exist!!\n", sizeof("Username already exist!!\n"));
		AdminMenu(client_socket);
	}	
}

void AdminLogin(int client_socket)
{
	char username[40], password[40];
	int flag=1;													//using flag to keep track of what menu to print
	read(client_socket, username, sizeof(username));
	read(client_socket, password, sizeof(password));
	if(!strcmp(username,"admin") && !strcmp(password,"root"))
	{		
		write(client_socket, &flag, sizeof(flag));							//login success, now print admin menu
		write(client_socket,"Welcome Admin!\n", sizeof("Welcome Admin!\n"));
		AdminMenu(client_socket);
	}
	else
	{	
		flag=0;												//login not successful, keep on welcome page only
		write(client_socket, &flag, sizeof(flag));
		write(client_socket,"Invalid Credentials!!\n", sizeof("Invalid Credentials!!\n"));		
		WelcomeMenu(client_socket);
	}
}
void ViewCustomerAdmin(int client_socket,char username[])
{
	struct Single_Customer db;
	int fd1 = open("Customer_Accounts", O_CREAT | O_APPEND | O_RDWR , 0666);
	while(read(fd1, (char *)&db, sizeof(struct Single_Customer)))
	{
		if(!strcmp(db.username, username))
		{
			lseek(fd1,-sizeof(struct Single_Customer),SEEK_CUR);
			memset(&db,0,sizeof(db));
			read(fd1,&db,sizeof(db));
			write(client_socket,&db,sizeof(db));
		}
	}
	close(fd1);
}
void ViewJointAdmin(int client_socket,char username1[],char username2[])
{
	struct Joint_Customer db;
	int fd1 = open("Joint_Accounts", O_CREAT | O_APPEND | O_RDWR, 0666);
	while(read(fd1, (char *)&db, sizeof(struct Joint_Customer)))
	{
		if((!strcmp(db.username1, username1)) && (!strcmp(db.username2,username2)) || (!strcmp(db.username1, username2)) && (!strcmp(db.username2,username1)))
		{
			lseek(fd1,-sizeof(struct Joint_Customer),SEEK_CUR);
			memset(&db,0,sizeof(db));
			read(fd1,&db,sizeof(db));
			write(client_socket,&db,sizeof(db));
		}
	}
	close(fd1);
}
void CustomerLogin(int client_socket)
{
	char username[40], password[40];
	struct flock lock;
	read(client_socket, username, sizeof(username));
	read(client_socket, password, sizeof(password));
	struct Single_Customer db;
	int flag=1;
	int fd1 = open("Customer_Accounts", O_CREAT | O_APPEND | O_RDWR , 0666);
	while(read(fd1, (char *)&db, sizeof(struct Single_Customer)))
	{
		if(!strcmp(db.username, username) && !strcmp(db.password, password))
			{
				lseek(fd1,-sizeof(struct Single_Customer),SEEK_CUR);
				lock.l_type=F_WRLCK;
				lock.l_whence=SEEK_CUR;
				lock.l_start=0;
				lock.l_len=sizeof(struct Single_Customer);
				//Lock Record
				fcntl(fd1,F_SETLKW,&lock);
				read(fd1,&db,sizeof(db));						//read here once again if some other user updated details
				write(client_socket, &flag, sizeof(flag));
				write(client_socket, "Login Successful!\n", sizeof("Login Successful!\n"));
				CustomerMenu(client_socket,db,lock,fd1);
				close(fd1);
				return;		
			}
	}
	close(fd1);
	flag=0;
	write(client_socket, &flag, sizeof(flag));
	write(client_socket, "Invalid Credentials!!\n", sizeof("Invalid Credentials!!\n"));
	WelcomeMenu(client_socket);	
}

void AddAccount(char username[], char password[],int client_socket)
{
	srand(time(0));
	long long int id;
	int fd1 = open("Customer_Accounts", O_CREAT | O_APPEND | O_RDWR , 0666);
	struct Single_Customer db;
	memset(&db,0,sizeof(struct Single_Customer));
	strcpy(db.username, username);
	strcpy(db.password, password);
	db.balance=0;
	id=(rand()%1500)+91300000000;				//randomly generate account number
	sprintf(db.account_no,"%lld",id);			//convert integer to string
	write(fd1, (char *)&db, sizeof(struct Single_Customer));
	write(client_socket, "Account Created Successfully!\n", sizeof("Account Created Successfully!\n"));
	close(fd1);
}

void UpdateCustomerDetails(struct Single_Customer db_user)
{
	struct Single_Customer db;
	int fd1 = open("Customer_Accounts", O_CREAT | O_RDWR , 0666);
	while(read(fd1, (char *)&db, sizeof (struct Single_Customer)))
	{
		if(!strcmp(db_user.username, db.username))
		{
			db.balance = db_user.balance;
			db.amount = db_user.amount;	
			strcpy(db.modified,db_user.modified);
			strcpy(db.date,db_user.date);
			strcpy(db.password,db_user.password);
			lseek(fd1,-sizeof(struct Single_Customer),SEEK_CUR);
			write(fd1, (char *)&db,sizeof(struct Single_Customer));
			break;
		}
	}
}

int CustomerAccountExists(char username[])
{
	struct Single_Customer db;
	int fd1 = open("Customer_Accounts", O_CREAT | O_APPEND | O_RDWR , 0666);
	while(read(fd1, (char *)&db, sizeof(struct Single_Customer)))
	{
		if(!strcmp(db.username, username))
			{
				close(fd1);
				return 1;
			}
	}
	close(fd1);
	return 0;	
}

void ViewCustomerDetails(int client_socket, struct Single_Customer db,struct flock lock,int fd)
{
	write(client_socket,db.username, sizeof(db.username));
	write(client_socket,db.account_no, sizeof(db.account_no));
	write(client_socket,&db.balance, sizeof(db.balance));
	write(client_socket,&db.amount, sizeof(db.amount));
	write(client_socket,db.modified,sizeof(db.modified));
	write(client_socket,db.date,sizeof(db.date));
	CustomerMenu(client_socket,db,lock,fd);
}

void CustomerDeposit(int client_socket, struct Single_Customer db,struct flock lock,int fd)
{
	int temp;
	char buffer1[20]="",buffer2[20]="";
	char ch = '-';
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	double amount;
	read(client_socket, &amount, sizeof(amount));

	db.amount = amount;								//set modified amount
	strcpy(db.modified,"Credited");					//set modification type		
	db.balance = db.balance + amount;
	temp = tm.tm_mday;
	sprintf(buffer1,"%d",temp);			//convert to string
	strncat(buffer2,buffer1,2);			//concat to date
	strncat(buffer2,&ch,1);
	temp = tm.tm_mon + 1;
	sprintf(buffer1,"%d",temp);
	strncat(buffer2,buffer1,2);
	strncat(buffer2,&ch,1);
	temp = tm.tm_year + 1900;
	sprintf(buffer1,"%d",temp);
	strncat(buffer2,buffer1,4);
	strcpy(db.date,buffer2);

    UpdateCustomerDetails(db);				//update the modified details
	write(client_socket, &db.balance, sizeof(db.balance));
	CustomerMenu(client_socket,db,lock,fd);
}

void CustomerWithdraw(int client_socket, struct Single_Customer db,struct flock lock,int fd)
{
	int temp;
	char ch = '-';
	char buffer1[20]="",buffer2[20]="";
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	double amount, flag=0;
	read(client_socket, &amount, sizeof(amount));
	if(amount >= db.balance)
	{
		write(client_socket,&flag,sizeof(flag));
		write(client_socket, &db.balance, sizeof(db.balance));
	}
	else
	{
		flag=1;

		db.amount = amount;								//set modified amount
		strcpy(db.modified,"Debited");					//set modification type	
		db.balance = db.balance - amount;
		temp = tm.tm_mday;
		sprintf(buffer1,"%d",temp);			//convert to string
		strncat(buffer2,buffer1,2);			//concat to date
		strncat(buffer2,&ch,1);
		temp = tm.tm_mon + 1;
		sprintf(buffer1,"%d",temp);
		strncat(buffer2,buffer1,2);
		strncat(buffer2,&ch,1);
		temp = tm.tm_year + 1900;
		sprintf(buffer1,"%d",temp);
		strncat(buffer2,buffer1,4);
		strcpy(db.date,buffer2);

		UpdateCustomerDetails(db);
		write(client_socket,&flag,sizeof(flag));
		write(client_socket, &db.balance, sizeof(db.balance));
	}
	CustomerMenu(client_socket,db,lock,fd);
}

void CustomerPassChange(int client_socket, struct Single_Customer db,struct flock lock,int fd)
{
	char password[40];
	read(client_socket,password,sizeof(password));
	strcpy(db.password,password);
	UpdateCustomerDetails(db);				//update password by over-writing
	WelcomeMenu(client_socket);
}

void DeleteAccount(int client_socket)		//to delete, we will write all data except data to be deleted into a new db file
{
	char username[40];
	int fd1, fd2;
	fd1 = open("Customer_Accounts", O_CREAT | O_RDWR , 0666);
	fd2 = open("Customer_Accounts_temp", O_CREAT | O_RDWR, 0666);		//new db file 
	read(client_socket, username, sizeof(username));
	if(!CustomerAccountExists(username))
	{
		write(client_socket, "Account does not exist with the given username!\n", sizeof("Account does not exist with the given username!\n"));
	}
	else
	{	
		struct Single_Customer db[1000], db1;			//create an array of struct to store db info temporarily
		int i=0;
		lseek(fd1, 0, SEEK_SET);											//seek to the beginnning	
		while(read(fd1, (char *)&db[i++], sizeof(struct Single_Customer)));		//read all data into struct db[] array
		for(int j = 0 ; j < i-1 ; j++)									//now, loop in struct db[] array
		{ 
			if(!strcmp(db[j].username, username))					//if record to be deleted found, don't do anything
			{
				continue;				
			}
			else											//otherwise, copy it into new database file
			{
				write(fd2,(char *)&db[j],sizeof(struct Single_Customer));
				
			}
		}
		lseek(fd2, 0, SEEK_SET);							//move pointer to start in new db file
		fd1=open("Customer_Accounts",O_TRUNC|O_RDWR,0666);		//truncate the existing db file
		lseek(fd1, 0, SEEK_SET);							//move pointer to start in existing db file
		while(read(fd2, (char *)&db1, sizeof(struct Single_Customer)))
		{
			write(fd1,(char *)&db1,sizeof(struct Single_Customer));		//copy back the new data to existing db file
		}
		lseek(fd1,0, SEEK_SET);
		write(client_socket,"Account Successfully Deleted!\n", sizeof("Account Successfully Deleted!\n"));
		open("Customer_Accounts_temp",O_TRUNC|O_RDWR,0666);				//truncate the temporarily created db file 
	}
	close(fd1);
	close(fd2);
	AdminMenu(client_socket);
}

void JointUpdateDetails(struct Joint_Customer db_user)
{
	struct Joint_Customer db;
	int fd1 = open("Joint_Accounts", O_CREAT | O_RDWR , 0666);
	while(read(fd1, (char *)&db, sizeof (struct Joint_Customer)))
	{
		if(!strcmp(db_user.username1, db.username1) && !strcmp(db_user.username2, db.username2))
		{
			db.balance = db_user.balance;
			db.amount = db_user.amount;
			strcpy(db.modified,db_user.modified);
			strcpy(db.date,db_user.date);
			strcpy(db.password1,db_user.password1);
			strcpy(db.password2,db_user.password2);

			lseek(fd1,-sizeof(struct Joint_Customer),SEEK_CUR);				
			write(fd1, (char *)&db,sizeof(struct Joint_Customer));
			break;
		}
	}
}

int JointAccountExists(char username1[],char username2[])
{
	struct Joint_Customer db;
	int fd1 = open("Joint_Accounts", O_CREAT | O_APPEND | O_RDWR, 0666);
	while(read(fd1, (char *)&db, sizeof(struct Joint_Customer)))
	{
		if((!strcmp(db.username1, username1)) && (!strcmp(db.username2,username2)) || (!strcmp(db.username1, username2)) && (!strcmp(db.username2,username1)))
			{
				close(fd1);
				return 1;
			}
	}
	close(fd1);
	return 0;	
}

void JointAccountMenu(int client_socket,struct Joint_Customer db,struct flock lock,int fd)
{
	int choice;
	read(client_socket, &choice, sizeof(choice));
	switch(choice)
	{
		case 1: ViewJointDetails(client_socket,db,lock,fd);
				break;
		case 2: JointDeposit(client_socket,db,lock,fd);
				break;
		case 3: JointWithdraw(client_socket,db,lock,fd);
				break;
		case 4: JointPassChange(client_socket,db,lock);
				break;
		case 5: lock.l_type=F_UNLCK;
				fcntl(fd,F_SETLK,&lock); 
				WelcomeMenu(client_socket);
				break;
	}
}

void addjointAccount(char username1[], char password1[], char username2[], char password2[], int client_socket)
{
	srand(time(0));
	long long int id;
	int fd1 = open("Joint_Accounts", O_CREAT | O_APPEND | O_RDWR , 0666);
	struct Joint_Customer db;
	strcpy(db.username1, username1);
	strcpy(db.password1, password1);
	strcpy(db.username2, username2);
	strcpy(db.password2, password2);
	db.balance=0;
	id=(rand()%1500)+51400000000;					//randomly generate account number
	sprintf(db.account_no,"%lld",id);			//convert integer to string
	write(fd1, (char *)&db, sizeof(struct Joint_Customer));
	write(client_socket, "Account Successfully Created!\n", sizeof("Account Successfully Created!\n"));
	close(fd1);
}

void NewJointAccount(int client_socket)						//called from admin function, so flag = 0
{
	char username1[40], password1[40], username2[40], password2[40];
	//read joint user unfo from client
	read(client_socket, username1, sizeof(username1));
	read(client_socket, password1, sizeof(password1));
	read(client_socket, username2, sizeof(username2));
	read(client_socket, password2, sizeof(password2));
	//check if joint account already exists or not
	if(!JointAccountExists(username1,username2))
	{
		addjointAccount(username1, password1, username2, password2, client_socket);		//add joint account
		AdminMenu(client_socket);
	}
	else
	{
		write(client_socket, "Username already exist!!\n", sizeof("Username already exist!!\n"));
		AdminMenu(client_socket);
	}
}

void JointAccountLogin(int client_socket)
{
	char username[40], password[40];
	struct flock lock;

	read(client_socket, username, sizeof(username));
	read(client_socket, password, sizeof(password));
	strcpy(cur_user,username);
	struct Joint_Customer db;
	int flag=1;
	int fd1 = open("Joint_Accounts", O_CREAT | O_APPEND | O_RDWR , 0666);
	while(read(fd1, (char *)&db, sizeof(struct Joint_Customer)))
	{
		if((!strcmp(db.username1, username) && !strcmp(db.password1,password)) || (!strcmp(db.username2, username) && !strcmp(db.password2, password)))
			{
				lseek(fd1,-sizeof(struct Joint_Customer),SEEK_CUR);
				lock.l_type=F_WRLCK;
				lock.l_whence=SEEK_CUR;
				lock.l_start=0;
				lock.l_len=sizeof(struct Joint_Customer);
				//Lock Record
				fcntl(fd1,F_SETLKW,&lock);
				read(fd1,&db,sizeof(db));						//read here once again if some other user updated details 
				write(client_socket, &flag, sizeof(flag));
				write(client_socket, "Login Successful!\n", sizeof("Login Successful!\n"));
				JointAccountMenu(client_socket,db,lock,fd1);				
				close(fd1);			
				return;		
			}
	}
	close(fd1);
	flag=0;
	write(client_socket, &flag, sizeof(flag));
	write(client_socket, "Invalid Credentials!!\n", sizeof("Invalid Credentials!!\n"));
	WelcomeMenu(client_socket);
}

void ViewJointDetails(int client_socket, struct Joint_Customer db,struct flock lock,int fd)
{
	write(client_socket,db.username1, sizeof(db.username1));
	write(client_socket,db.username2, sizeof(db.username2));
	write(client_socket,db.account_no, sizeof(db.account_no));
	write(client_socket,&db.balance, sizeof(db.balance));
	write(client_socket,&db.amount, sizeof(db.amount));
	write(client_socket,db.modified, sizeof(db.modified));
	write(client_socket,db.date, sizeof(db.date));
	JointAccountMenu(client_socket,db,lock,fd);
}

void JointDeposit(int client_socket, struct Joint_Customer db,struct flock lock,int fd)
{
	int temp;
	char buffer1[20]="",buffer2[20]="";
	char ch = '-';
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	double amount;
	read(client_socket, &amount, sizeof(amount));

	db.amount = amount;								//set modified amount
	strcpy(db.modified,"Credited");					//set modification type
	db.balance = db.balance + amount;
	temp = tm.tm_mday;
	sprintf(buffer1,"%d",temp);			//convert to string
	strncat(buffer2,buffer1,2);			//concat to date
	strncat(buffer2,&ch,1);
	temp = tm.tm_mon + 1;
	sprintf(buffer1,"%d",temp);
	strncat(buffer2,buffer1,2);
	strncat(buffer2,&ch,1);
	temp = tm.tm_year + 1900;
	sprintf(buffer1,"%d",temp);
	strncat(buffer2,buffer1,4);
	strcpy(db.date,buffer2);

    JointUpdateDetails(db);
	write(client_socket, &db.balance, sizeof(db.balance));
	JointAccountMenu(client_socket,db,lock,fd);
}

void JointWithdraw(int client_socket, struct Joint_Customer db,struct flock lock, int fd)
{
	int temp;
	char buffer1[20]="",buffer2[20]="";
	char ch = '-';
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	double amount, flag=0;
	read(client_socket, &amount, sizeof(amount));
	if(amount >= db.balance)
	{
		write(client_socket,&flag,sizeof(flag));
		write(client_socket, &db.balance, sizeof(db.balance));
	}
	else
	{
		flag=1;
		db.amount = amount;								//set modified amount
		strcpy(db.modified,"Debited");					//set modification type
		db.balance = db.balance - amount;
		temp = tm.tm_mday;
		sprintf(buffer1,"%d",temp);			//convert to string
		strncat(buffer2,buffer1,2);			//concat to date
		strncat(buffer2,&ch,1);
		temp = tm.tm_mon + 1;
		sprintf(buffer1,"%d",temp);
		strncat(buffer2,buffer1,2);
		strncat(buffer2,&ch,1);
		temp = tm.tm_year + 1900;
		sprintf(buffer1,"%d",temp);
		strncat(buffer2,buffer1,4);
		strcpy(db.date,buffer2);

		JointUpdateDetails(db);
		write(client_socket,&flag,sizeof(flag));
		write(client_socket, &db.balance, sizeof(db.balance));
	}
	JointAccountMenu(client_socket,db,lock,fd);
}

void JointPassChange(int client_socket, struct Joint_Customer db,struct flock lock)
{
	char password[40];
	read(client_socket,password,sizeof(password));
    if(!strcmp(cur_user,db.username1))
	   strcpy(db.password1,password);
	else
		strcpy(db.password2,password);
	JointUpdateDetails(db);
	WelcomeMenu(client_socket);
}

void DeleteJointAccount(int client_socket)			//same delete logic for join account also
{
	char username1[40], username2[40];
	int fd1, fd2;
	fd1 = open("Joint_Accounts", O_CREAT | O_RDWR , 0666);
	fd2 = open("Joint_Accounts_temp", O_CREAT | O_RDWR, 0666);
	read(client_socket, username1, sizeof(username1));
	read(client_socket, username2, sizeof(username2)); 
	if(!JointAccountExists(username1,username2))
	{	
		write(client_socket, "Account does not exist with the given usernames\n", sizeof("Account does not exist with the given usernames\n"));
	}
	else
	{		
		struct Joint_Customer db[1000], db1;
		int i=0;
		lseek(fd1, 0, SEEK_SET);
		while(read(fd1, (char *)&db[i++], sizeof(struct Joint_Customer)));	
		for(int j = 0; j < i-1; j++)
		{		
			if((!strcmp(db[j].username1, username1)) && (!strcmp(db[j].username2,username2)) || (!strcmp(db[j].username1, username2)) && (!strcmp(db[j].username2,username1)))
			{
				continue;				
			}	
			else
			{
				write(fd2,(char *)&db[j],sizeof(struct Joint_Customer));
				
			}
		}
		lseek(fd2, 0, SEEK_SET);
		fd1=open("Joint_Accounts",O_TRUNC|O_RDWR,0666);
		lseek(fd1, 0, SEEK_SET);		
		while(read(fd2, (char *)&db1, sizeof(struct Joint_Customer)))
		{
			write(fd1,(char *)&db1,sizeof(struct Joint_Customer));
		}
		lseek(fd1,0, SEEK_SET);

		write(client_socket,"Account deleted Successfully!\n", sizeof("Account deleted Successfully!\n"));
		open("Joint_Accounts_temp",O_TRUNC|O_RDWR,0666);
	}
	close(fd1);
	close(fd2);
	AdminMenu(client_socket);
}
