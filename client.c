#include <sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<string.h>
#include "single_customer.h"
#include "joint_customer.h"

void WelcomeMenu(int sockfd);
void CustomerMenu(int sockfd);
void AdminMenu(int sockfd);
void JointAccountLogin(int sockfd);
void JointAccountMenu(int sockfd);
void DeleteAccount(int sockfd);
void DeleteJointAccount(int sockfd);
void NewJointAccount(int sockfd);
void AddAcount(int sockfd);
void CustomerLogin(int sockfd);
void ViewCustomerDetails(int sockfd);
void ViewJointDetails(int sockfd);
void CustomerDeposit(int sockfd);
void JointDeposit(int sockfd);
void JointWithdraw(int sockfd);
void CustomerWithdraw(int sockfd);
void JointPassChange(int sockfd);
void CustomerPassChange(int sockfd);
void AdminLogin(int sockfd);
void ViewCustomerAdmin(int sockfd);
void ViewJointAdmin(int sockfd);

int main()
{
	struct sockaddr_in server;
	int sockfd = socket(AF_INET, SOCK_STREAM,0);	//IPv4, reliable stream (tcp),
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;			//specify host interface
	server.sin_port = htons(49994);					//specify port number
	connect(sockfd, (struct sockaddr *)(&server), sizeof(server));
	//communicate with server
	WelcomeMenu(sockfd);
	close(sockfd);
	return 0;
}

void WelcomeMenu(int sockfd)
{
	int choice;
	printf("*********Welcome to Online Banking System*********\n");
	printf("1.)Customer Login\n2.)Administrator Login\n3.)Joint Account Customer Login\n4.Exit\n");
	printf("Enter your choice\n");
	scanf("%d",&choice);
	write(sockfd, &choice, sizeof(choice));
	switch(choice)
	{
		case 1: CustomerLogin(sockfd);					//customer login function
				break;
		case 2: AdminLogin(sockfd);				//admin login function	
				break;	
		case 3: JointAccountLogin(sockfd);		//joint account login function
				break;
		case 4: exit(0);
				break;
	}
}

void CustomerMenu(int sockfd)
{
	int choice;
	printf("***********Welcome Customer***********\n");
	printf("1.)View Account Details\n2).Deposit Amount\n3.)Withdraw Amount\n4.)Change Password\n5.)Exit\n");
	printf("Please Enter your choice\n");
	scanf("%d",&choice);
	write(sockfd, &choice, sizeof(choice));
	switch(choice)
	{
		case 1: ViewCustomerDetails(sockfd);
				break;
		case 2: CustomerDeposit(sockfd);
				break;
		case 3: CustomerWithdraw(sockfd);
				break;
		case 4: CustomerPassChange(sockfd);
				break;
		case 5: WelcomeMenu(sockfd);
				break;
		default:WelcomeMenu(sockfd);
				break;
	}
}

void AdminLogin(int sockfd)
{
	char username[40], password[40], buffer[20];
	int flag;	
	printf("Please Enter admin username\n");
	scanf("%s",username);
	strcpy(password,getpass("Please Enter admin password\n"));
	write(sockfd, username, sizeof(username));
	write(sockfd, password, sizeof(password));
	read(sockfd, &flag, sizeof(flag));				//read flag from server regarding authentication
	read(sockfd, buffer, sizeof(buffer));			//login prompt
	printf("%s\n",buffer);
	if(flag == 1)						//if login success, then admin menu
		AdminMenu(sockfd);
	else
		WelcomeMenu(sockfd);				//else, keep on welcome page only
}

void AdminMenu(int sockfd)
{
	int choice;
	printf("1.)Add New Customer Account\n2.)Delete Customer Account\n3.)Search Customer Account\n4.)Add New Joint Account\n5.)Delete Joint Account\n6.)Search for a Joint Account\n7.)Exit\n");
	printf("Please Enter your choice\n");
	scanf("%d", &choice);
	char buffer[100], username[40], password[40], username1[40], username2[40];
	write(sockfd, &choice, sizeof(choice));
	switch(choice)
	{
		case 1:
			AddAcount(sockfd);
			break;
		case 2:
			DeleteAccount(sockfd);
			break;
		case 3:
			printf("Please Enter the username you want to find\n");
			scanf("%s", username);
			write(sockfd, username, sizeof(username));
			read(sockfd, &buffer, sizeof(buffer));			//read if account found or not
			printf("%s\n", buffer);
			if(!strcmp(buffer,"Account Found!\n"))
				ViewCustomerAdmin(sockfd);
			AdminMenu(sockfd);
			break;
		case 4:
			NewJointAccount(sockfd);
			break;
		case 5:
			DeleteJointAccount(sockfd);
			break;
		case 6:
			printf("Please Enter username 1 of Joint account\n");
			scanf("%s", username1);
			printf("Please Enter username 2 of Joint account\n");
			scanf("%s", username2);
			write(sockfd, username1, sizeof(username1));
			write(sockfd, username2, sizeof(username2));
			read(sockfd, &buffer, sizeof(buffer));
			printf("%s\n", buffer);
			if(!strcmp(buffer,"Account Found!\n"))
				ViewJointAdmin(sockfd);
			AdminMenu(sockfd);
			break;
		default:
			WelcomeMenu(sockfd);
	}
}

void ViewCustomerAdmin(int sockfd)
{
	struct Single_Customer db;
	read(sockfd,&db,sizeof(db));
	printf("Customer's Username - %s\nCustomer's Account Number - %s\nCustomer's Account Balance - %lf\nCustomer's Last Transaction Type - %s\nCustomer's Last Transaction Amount - %lf\nCustomer's Last Transaction Date - %s\n",db.username,db.account_no,db.balance,db.modified,db.amount,db.date);
}
void ViewJointAdmin(int sockfd)
{
	struct Joint_Customer db;
	read(sockfd,&db,sizeof(db));
	printf("First Customer's Username - %s\nSecond Customer's Username - %s\nCustomer's Account Number - %s\nCustomer's Balance remaining - %lf\nCustomer's Last Transaction Type - %s\nCustomer's Last Transaction Amount - %lf\nCustomer's Last Transaction Date - %s\n",db.username1,db.username2,db.account_no,db.balance,db.modified,db.amount,db.date);
}

void DeleteAccount(int sockfd)
{
	char username[40],buffer[100];
	printf("Please Enter the username of the account to be deleted\n");
	scanf("%s", username);
	write(sockfd, username, sizeof(username));
	read(sockfd, buffer, sizeof(buffer));
	printf("%s\n",buffer);
	AdminMenu(sockfd);
}

void CustomerPassChange(int sockfd)
{
	char password[40];
	strcpy(password,getpass("Please Enter the new password\n"));
	write(sockfd,password,sizeof(password));
	WelcomeMenu(sockfd);	
}

void DeleteJointAccount(int sockfd)
{
	char username1[40], username2[40], buffer[100];
	printf("Please Enter both the joint account user's usernames\n");
	printf("Please Enter Username 1 to delete\n");
	scanf("%s",username1);
	printf("Please Enter Username 2 to delete\n");
	scanf("%s",username2);
	write(sockfd, username1, sizeof(username1));		//send username1 to server
	write(sockfd, username2, sizeof(username2));		//send username 2 to server
	read(sockfd, buffer, sizeof(buffer));
	printf("%s\n",buffer);
	AdminMenu(sockfd);
}

void NewJointAccount(int sockfd)
{
	char username1[40], password1[40], username2[40], password2[40], buffer[100];
	printf("Please Enter first username for joint account\n");
	scanf("%s", username1);
	strcpy(password1,getpass("Please Enter password for first user of joint account\n"));
	printf("Please Enter second username for joint account\n");
	scanf("%s", username2);
	strcpy(password2,getpass("Please Enter password for second user of joint account\n"));
	write(sockfd, username1, sizeof(username1));
	write(sockfd, password1, sizeof(password1));
	write(sockfd, username2, sizeof(username2));
	write(sockfd, password2, sizeof(password2));
	read(sockfd, buffer, sizeof(buffer));
	printf("%s\n", buffer);
	AdminMenu(sockfd);
}

void AddAcount(int sockfd)
{
	int account_id;
	char username[40],password[40], buffer[100];
	printf("Please Enter the Customer's username\n");
	scanf("%s",username);
	strcpy(password,getpass("Please Enter Customer's Password\n"));
	write(sockfd, username,sizeof(username));
	write(sockfd, password, sizeof(password));
	read(sockfd, buffer, sizeof(buffer));
	printf("%s\n", buffer);
	AdminMenu(sockfd);
}

void CustomerLogin(int sockfd)
{
	char buffer[100];
	char username[40],password[40];
	int flag;
	printf("Please Enter your username\n");
	scanf("%s",username);
	strcpy(password,getpass("Please Enter your password\n"));
	write(sockfd, username,sizeof(username));
	write(sockfd, password, sizeof(password));
	read(sockfd, &flag, sizeof(flag));
	read(sockfd, buffer, sizeof(buffer));
	printf("%s\n",buffer);
	if(flag==1)
		CustomerMenu(sockfd);
	else
		WelcomeMenu(sockfd);
}

void JointAccountLogin(int sockfd)
{
	char buffer[100];
	char username[40],password[40];
	int flag;
	printf("Please Enter the username for joint account\n");
	scanf("%s",username);
	strcpy(password,getpass("Please Enter password for joint account\n"));
	//send login credentials for verification to server
	write(sockfd, username,sizeof(username));
	write(sockfd, password, sizeof(password));
	//read flag and message received
	read(sockfd, &flag, sizeof(flag));
	read(sockfd, buffer, sizeof(buffer));
	printf("%s\n",buffer);
	if(flag==1)
	{
		JointAccountMenu(sockfd);
	}
	else
		WelcomeMenu(sockfd);
}

void ViewCustomerDetails(int sockfd)
{
	char username[40],account_no[40],modified[10],date[20];
	double balance,amount;
	read(sockfd,username, sizeof(username));
	read(sockfd,account_no, sizeof(account_no));
	read(sockfd,&balance, sizeof(balance));
	read(sockfd,&amount, sizeof(amount));
	read(sockfd,modified, sizeof(modified));
	read(sockfd,date, sizeof(date));
	printf("Customer's Username - %s\nCustomer's Account Number - %s\nCustomer's Account Balance - %lf\nCustomer's Last Transaction Type - %s\nCustomer's Last Transaction Amount - %lf\nCustomer's Last Transaction Date - %s\n",username,account_no,balance,modified,amount,date);
	CustomerMenu(sockfd);
}

void ViewJointDetails(int sockfd)
{
	char username1[40], username2[40],account_no[40],modified[10],date[20];
	double balance,amount;
	read(sockfd,username1, sizeof(username1));
	read(sockfd,username2, sizeof(username2));
	read(sockfd,account_no, sizeof(account_no));
	read(sockfd,&balance, sizeof(balance));
	read(sockfd,&amount, sizeof(amount));
	read(sockfd,modified, sizeof(modified));
	read(sockfd,date, sizeof(date));
	printf("First Customer's Username - %s\nSecond Customer's Username - %s\nCustomer's Account Number - %s\nCustomer's Balance remaining - %lf\nCustomer's Last Transaction Type - %s\nCustomer's Last Transaction Amount - %lf\nCustomer's Last Transaction Date - %s\n",username1,username2,account_no,balance,modified,amount,date);
	JointAccountMenu(sockfd);
}

void CustomerDeposit(int sockfd)
{
	double amount;
	printf("Please enter the amount to be deposited\n");
	scanf("%lf",&amount);
	write(sockfd, &amount, sizeof(amount));
	read(sockfd, &amount, sizeof(amount));		//read new updated balance
	printf("Customer's New Balance - %lf\n",amount);
	CustomerMenu(sockfd);
}

void JointDeposit(int sockfd)
{
	double amount;
	printf("Please enter the amount to be deposited into the joint account\n");
	scanf("%lf",&amount);
	write(sockfd, &amount, sizeof(amount));
	read(sockfd, &amount, sizeof(amount));
	printf("Customer's New Balance - %lf\n",amount);
	JointAccountMenu(sockfd);
}

void JointWithdraw(int sockfd)
{
	double amount,flag;
	printf("Please Enter amount to withdraw\n");
	scanf("%lf",&amount);
	write(sockfd, &amount, sizeof(amount));
	read(sockfd, &flag, sizeof(flag));
	if(flag==0)
	{
		printf("Insufficient Balance in Customer's account!!\n");
		read(sockfd, &amount, sizeof(amount));
		printf("Customer's Balance - %lf\n",amount);
	}
	else
	{
		read(sockfd, &amount, sizeof(amount));
		printf("Customer's NewBalance - %lf\n",amount);

	}
	JointAccountMenu(sockfd);
}

void CustomerWithdraw(int sockfd)
{
	double amount,flag;
	printf("Please Enter amount to withdraw\n");
	scanf("%lf",&amount);
	write(sockfd, &amount, sizeof(amount));
	read(sockfd, &flag, sizeof(flag));
	if(flag==0)
	{
		printf("Insufficient Balance in Customer's account!!\n");
		read(sockfd, &amount, sizeof(amount));
		printf("Customer's Balance - %lf\n",amount);
	}
	else
	{
		read(sockfd, &amount, sizeof(amount));
		printf("Customer's NewBalance - %lf\n",amount);
	}
	CustomerMenu(sockfd);
}

void JointPassChange(int sockfd)
{
	char password[40];
	strcpy(password,getpass("Please Enter the new password\n"));
	write(sockfd,password,sizeof(password));
	WelcomeMenu(sockfd);
}

void JointAccountMenu(int sockfd)
{
	int choice;
	printf("1.)View Customer's Account Details\n2.)Deposit Amount\n3.)Withdraw Amount\n4.)Change Password\n5.)Exit\n");
	printf("Please Enter your choice\n");
	scanf("%d",&choice);
	write(sockfd, &choice, sizeof(choice));
	switch(choice)
	{
		case 1: ViewJointDetails(sockfd);
				break;
		case 2: JointDeposit(sockfd);
				break;
		case 3: JointWithdraw(sockfd);
				break;
		case 4: JointPassChange(sockfd);
				break;
		case 5: WelcomeMenu(sockfd);
				break;
	}
}
