/*
Client side:
socket->connect->send/recv->close
*/

#include <string.h>
#include <sys/socket.h> 
#include <unistd.h> 
#include <arpa/inet.h> 
#include <stdio.h>
#include <stdlib.h> 
	 
#define PORT 8881

//------------------------------FUNCTION DECLERATIONS---------------------------------------//

int client(int sock);
int menu(int sock,int type);
int user_function(int sock,int choice);
int crud_train(int sock,int choice);
int crud_user(int sock,int choice);

int main(void) { 
	int sock; 
    	struct sockaddr_in server; 
    	char server_reply[50],*server_ip;
	server_ip = "127.0.0.1"; 
     
    	sock = socket(AF_INET, SOCK_STREAM, 0); 
    	if (sock == -1) { 
       	printf("Could not create socket"); 
    	} 
    
    	server.sin_addr.s_addr = inet_addr(server_ip); 
    	server.sin_family = AF_INET; 
    	server.sin_port = htons(PORT); 
   
    	if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0)
       	perror("connect failed. Error"); 
    
	while(client(sock)!=3);
    	close(sock); 
    	
	return 0; 
} 

//-------------------- FIRST FUNCTION THAT IS CALLED-----------------------------//

int client(int sock){
	int choice,valid;
	//system("clear");
	printf("\n\n\t\t\tTRAIN RESERVATION SYSTEM\n\n");
	printf("\t1. Sign In\n");
	printf("\t2. Sign Up\n");
	printf("\t3. Exit\n");
	printf("\tEnter Your Choice:: ");
	scanf("%d", &choice);
	write(sock, &choice, sizeof(choice));
	if (choice == 1){					// LOG IN
		int id,type;
		char password[50];
		printf("\tlogin id:: ");
		scanf("%d", &id);
		strcpy(password,getpass("\tpassword:: "));
		write(sock, &id, sizeof(id));
		write(sock, &password, sizeof(password));
		read(sock, &valid, sizeof(valid));
		if(valid){
			printf("\tlogin successfully\n");
			read(sock,&type,sizeof(type));
			while(menu(sock,type)!=-1);
			//system("clear");
			return 1;
		}
		else{
			printf("\tLogin Failed : Incorrect password or login id\n");
			return 1;
		}
	}

	else if(choice == 2){					// SIGN UP
		int type,id;
		char name[50],password[50],secret_pin[6];
		system("clear");
		printf("\n\tEnter The Type Of Account:: \n");
		printf("\t0. Admin\n\t1. Agent\n\t2. Customer\n");
		printf("\tYour Response: ");
		scanf("%d", &type);
		printf("\tEnter Your Name: ");
		scanf("%s", name);
		strcpy(password,getpass("\tEnter Your Password: "));

		if(!type){
			while(1){
				strcpy(secret_pin, getpass("\tEnter secret PIN to create ADMIN account: "));
				if(strcmp(secret_pin, "secret")!=0) 					
					printf("\tInvalid PIN. Please Try Again.\n");
				else
					break;
			}
		}

		write(sock, &type, sizeof(type));
		write(sock, &name, sizeof(name));
		write(sock, &password, strlen(password));
		
		read(sock, &id, sizeof(id));
		printf("\tRemember Your login id For Further Logins as :: %d\n", id);
		return 2;
	}
	else							// LOG OUT
		return 3;
	
}

//-------------------- MAIN MENU FUNCTION----------------------------//

int menu(int sock,int type){
	int choice;
	if(type==2 || type==1){					// AGENT AND CUSTOMER
		printf("\t1. Book Ticket\n");
		printf("\t2. View Bookings\n");
		printf("\t3. Update Booking\n");
		printf("\t4. Cancel booking\n");
		printf("\t5. Logout\n");
		printf("\tYour Choice: ");
		scanf("%d",&choice);
		write(sock,&choice,sizeof(choice));
		return user_function(sock,choice);
	}
	else if(type==0){					// ADMIN
		printf("\n\t1. CRUD operations on train\n");
		printf("\t2. CRUD operations on user\n");
		printf("\t3. Logout\n");
		printf("\t Your Choice: ");
		scanf("%d",&choice);
		write(sock,&choice,sizeof(choice));
			if(choice==1){
				printf("\t1. Add train\n");
				printf("\t2. View train\n");
				printf("\t3. Modify train\n");
				printf("\t4. Delete train\n");
				printf("\t Your Choice: ");
				scanf("%d",&choice);	
				write(sock,&choice,sizeof(choice));
				return crud_train(sock,choice);
			}
			else if(choice==2){
				printf("\t1. Add User\n");
				printf("\t2. View all users\n");
				printf("\t3. Modify user\n");
				printf("\t4. Delete user\n");
				printf("\t Your Choice: ");
				scanf("%d",&choice);
				write(sock,&choice,sizeof(choice));
				return crud_user(sock,choice);
			
			}
			else if(choice==3)
				return -1;
	}	
	
}

//--------------------------------CRUD OPERATIONS TRAIN----------------------------//
int crud_train(int sock,int choice){
	int valid = 0;
	if(choice==1){				                       // ADD TRAIN RESPONSE
		char tname[50];
		printf("\n\tEnter train name: ");
		scanf("%s",tname);
		write(sock, &tname, sizeof(tname));
		read(sock,&valid,sizeof(valid));	
		if(valid)
			printf("\n\tTrain added successfully\n");

		return valid;	
	}
	
	else if(choice==2){			                   // VIEW TRAIN RESPONSE
		int no_of_trains;
		int tno;
		char tname[50];
		int tseats;
		int aseats;
		read(sock,&no_of_trains,sizeof(no_of_trains));

		printf("\tT_no\tT_name\tT_seats\tA_seats\n");
		while(no_of_trains--){
			read(sock,&tno,sizeof(tno));
			read(sock,&tname,sizeof(tname));
			read(sock,&tseats,sizeof(tseats));
			read(sock,&aseats,sizeof(aseats));
			
			if(strcmp(tname, "deleted")!=0)
				printf("\t%d\t%s\t%d\t%d\n",tno,tname,tseats,aseats);
		}

		return valid;	
	}
	
	else if (choice==3){			                  // UPDATE TRAIN RESPONSE
		int tseats,choice=2,valid=0,tid;
		char tname[50];
		write(sock,&choice,sizeof(int));
		crud_train(sock,choice);
		printf("\n\t Enter the train number you want to modify: ");
		scanf("%d",&tid);
		write(sock,&tid,sizeof(tid));
		
		printf("\n\t1. Train Name\n\t2. Total Seats\n");
		printf("\t Your Choice: ");
		scanf("%d",&choice);
		write(sock,&choice,sizeof(choice));
		
		if(choice==1){
			read(sock,&tname,sizeof(tname));
			printf("\n\t Current name: %s",tname);
			printf("\n\t Updated name:");
			scanf("%s",tname);
			write(sock,&tname,sizeof(tname));
		}
		else if(choice==2){
			read(sock,&tseats,sizeof(tseats));
			printf("\n\t Current value: %d",tseats);
			printf("\n\t Updated value:");
			scanf("%d",&tseats);
			write(sock,&tseats,sizeof(tseats));
		}
		read(sock,&valid,sizeof(valid));
		if(valid)
			printf("\n\t Train data updated successfully\n");
		return valid;
	}

	else if(choice==4){				                     // DELETE TRAIN RESPONSE
		int choice=2,tid,valid=0;
		write(sock,&choice,sizeof(int));
		crud_train(sock,choice);
		
		printf("\n\t Enter the train number you want to delete: ");
		scanf("%d",&tid);
		write(sock,&tid,sizeof(tid));
		read(sock,&valid,sizeof(valid));
		if(valid)
			printf("\n\t Train deleted successfully\n");
		return valid;
	}
	
}

//-------------------------------- CRUD OPERATIONS USER-----------------------------//

int crud_user(int sock,int choice){
	int valid = 0;
	if(choice==1){							// ADD USER
		int type,id;
		char name[50],password[50];
		printf("\n\tEnter The Type Of Account:: \n");
		printf("\t1. Agent\n\t2. Customer\n");
		printf("\tYour Response: ");
		scanf("%d", &type);
		printf("\tUser Name: ");
		scanf("%s", name);
		strcpy(password,getpass("\tPassword: "));
		write(sock, &type, sizeof(type));
		write(sock, &name, sizeof(name));
		write(sock, &password, strlen(password));
		read(sock,&valid,sizeof(valid));	
		if(valid){
			read(sock,&id,sizeof(id));
			printf("\tRemember Your login id For Further Logins as :: %d\n", id);
		}
		return valid;	
	}
	
	else if(choice==2){						// VIEW USER LIST
		int no_of_users;
		int id,type;
		char uname[50];
		read(sock,&no_of_users,sizeof(no_of_users));

		printf("\tU_id\tU_name\tU_type\n");
		while(no_of_users--){
			read(sock,&id,sizeof(id));
			read(sock,&uname,sizeof(uname));
			read(sock,&type,sizeof(type));
			
			if(strcmp(uname, "deleted")!=0)
				printf("\t%d\t%s\t%d\n",id,uname,type);
		}

		return valid;	
	}

	else if (choice==3){						// UPDATE USER
		int choice=2,valid=0,uid;
		char name[50],pass[50];
		write(sock,&choice,sizeof(int));
		crud_user(sock,choice);
		printf("\n\t Enter the U_id you want to modify: ");
		scanf("%d",&uid);
		write(sock,&uid,sizeof(uid));
		
		printf("\n\t1. User Name\n\t2. Password\n");
		printf("\t Your Choice: ");
		scanf("%d",&choice);
		write(sock,&choice,sizeof(choice));
		
		if(choice==1){
			read(sock,&name,sizeof(name));
			printf("\n\t Current name: %s",name);
			printf("\n\t Updated name:");
			scanf("%s",name);
			write(sock,&name,sizeof(name));
			read(sock,&valid,sizeof(valid));
		}
		else if(choice==2){
			printf("\n\t Enter Current password: ");
			scanf("%s",pass);
			write(sock,&pass,sizeof(pass));
			read(sock,&valid,sizeof(valid));
			if(valid){
				printf("\n\t Enter new password:");
				scanf("%s",pass);
			}
			else
				printf("\n\tIncorrect password\n");
			
			write(sock,&pass,sizeof(pass));
		}
		if(valid){
			read(sock,&valid,sizeof(valid));
			if(valid)
				printf("\n\t User data updated successfully\n");
		}
		return valid;
	}

	else if(choice==4){						// DELETE A USER
		int choice=2,uid,valid=0;
		write(sock,&choice,sizeof(int));
		crud_user(sock,choice);
		
		printf("\n\t Enter the id you want to delete: ");
		scanf("%d",&uid);
		write(sock,&uid,sizeof(uid));
		read(sock,&valid,sizeof(valid));
		if(valid)
			printf("\n\t User deleted successfully\n");
		return valid;
	}
}

//-------------------------------- USER FUNCTION TO BOOK TICKETS -----------------------------//
int user_function(int sock,int choice){
	int valid =0;
	if(choice==1){										// BOOK TICKETS
		int view=2,tid,seats;
		write(sock,&view,sizeof(int));
		crud_train(sock,view);
		printf("\n\tEnter the train number you want to book: ");
		scanf("%d",&tid);
		write(sock,&tid,sizeof(tid));
				
		printf("\n\tEnter the no. of seats you want to book: ");
		scanf("%d",&seats);
		write(sock,&seats,sizeof(seats));
	
		read(sock,&valid,sizeof(valid));
		if(valid)
			printf("\n\tTicket booked successfully.\n");
		else
			printf("\n\tSeats were not available.\n");

		return valid;
	}
	
	else if(choice==2){									// VIEW TICKETS
		int no_of_bookings;
		int id,tid,seats;
		read(sock,&no_of_bookings,sizeof(no_of_bookings));

		printf("\tB_id\tT_no\tSeats\n");
		while(no_of_bookings--){
			read(sock,&id,sizeof(id));
			read(sock,&tid,sizeof(tid));
			read(sock,&seats,sizeof(seats));
			
			if(seats!=0)
				printf("\t%d\t%d\t%d\n",id,tid,seats);
		}

		return valid;
	}

	else if(choice==3){									//UPDATE A BOOKING (INCREMENT/ DECREMENT SEATS)
		int choice = 2,bid,val,valid;
		user_function(sock,choice);
		printf("\n\t Enter the B_id you want to modify: ");
		scanf("%d",&bid);
		write(sock,&bid,sizeof(bid));

		printf("\n\t1. Increase number of seats\n\t2. Decrease number of seats\n");
		printf("\t Your Choice: ");
		scanf("%d",&choice);
		write(sock,&choice,sizeof(choice));

		if(choice==1){
			printf("\n\tNo. of tickets to increase");
			scanf("%d",&val);
			write(sock,&val,sizeof(val));
		}
		else if(choice==2){
			printf("\n\tNo. of tickets to decrease");
			scanf("%d",&val);
			write(sock,&val,sizeof(val));
		}
		read(sock,&valid,sizeof(valid));
		if(valid)
			printf("\n\tBooking updated successfully.\n");
		else
			printf("\n\tUpdation failed. No more seats available.\n");
		return valid;
	}
	
	else if(choice==4){									// CANCEL THE ENTIRE BOOKING
		int choice = 2,bid,valid;
		user_function(sock,choice);
		printf("\n\t Enter the B_id you want to cancel: ");
		scanf("%d",&bid);
		write(sock,&bid,sizeof(bid));
		read(sock,&valid,sizeof(valid));
		if(valid)
			printf("\n\tBooking cancelled successfully.\n");
		else
			printf("\n\tCancellation failed.\n");
		return valid;
	}
	else if(choice==5)									// LOGOUT
		return -1;
	
}
