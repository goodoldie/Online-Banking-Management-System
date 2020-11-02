# Online-Banking-Management-System
A Unix terminal based project which implements online banking using system calls, predominantly. Socket programming is used to communicate with the server for handling transactions and database updates. File locking has been implemented to prevent concurrent access to a particular user account.

This project has been created to implement a user friendly online banking system. It runs on a Unix Terminal.
I do not provide any WARRANTY for this software. Please use and modify this at your own discretion.

How to run the program?

On terminal 1 -> gcc -o server server.c
				 then run as ./server
On terminal 2 -> gcc -o client client.c
				 then run as ./client
         
Admin Credentials:
username - admin
password - root

Initially, no database and files have been created. They will be created as and when necessary at certain steps.
Separate structures have been implemented for joint users and normal users for the ease of implementation.
Socket programming has been used to communicate between client and server. All the transactions and database updations have been done on the server side only. The client sends all the data given by the user to the server.

File locking has been implemented to prevent simultaneous read/write access.
At a time, only one instance of customer can log in to the server.
In case of joint account, if one of the account holders has logged in, the other user of that joint account cannot log in to the server.

Necessary information has been stored in the database files which are separate for normal customers and joint account customers, i.e, Customer_Accounts and Joint_Accounts. A separate temporary DB file has also been created to facilitate the deletion of records from the database.
