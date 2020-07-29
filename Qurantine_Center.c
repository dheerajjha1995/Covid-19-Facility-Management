#include <stdio.h>
#include <stdlib.h>//to use exit()
#include <conio.h>//to use getch(), clrscr()
#include <ctype.h>
#include <string.h>
#define MAX_YR  9999
#define MIN_YR  1900
#define MAX_SIZE_USER_NAME 30
#define MAX_SIZE_PASSWORD  20
#define PATIENT_FILE "PatientData.bin"
// Macro related to the patients info
#define MAX_PATIENT_NAME 50
#define MAX_ADDRESS 300
#define TOTAL_ROOMS 323
#define FILE_HEADER_SIZE  sizeof(sFileHeader)
int patientCount = 0;  //To keep a count of patient admitted

//structure to store date
typedef struct
{
    int yyyy;
    int mm;
    int dd;
} Date;

//structure to store password info
typedef struct
{
    char username[MAX_SIZE_USER_NAME];
    char password[MAX_SIZE_PASSWORD];
} sFileHeader;

//structure to hold patients info
typedef struct
{
    unsigned int patientsId; // patient id which is always +ve
    char patientName[MAX_PATIENT_NAME];
    int roomNo;
    int patientAge;
    char patientAddr[MAX_ADDRESS];
    char patientComingFrom[MAX_ADDRESS];
    char patientGoingTo[MAX_ADDRESS];
    Date patientAdmitDate;
    Date patientDischargeDate;
} patient;

patient patientLoaded[TOTAL_ROOMS]; //patients database in memory during runtime

//structure to hold room data
typedef struct
{
    int patientsId;
    int occupied;
} room;

room roomLoaded[TOTAL_ROOMS]; //room info in memory during runtime

//intialize every room as unoccupied as the program loads
//which is further updated from the database file
void roomInitialization()
{
    for(int i = 0; i < TOTAL_ROOMS; i++)
    {
        roomLoaded[i].occupied = 0;
    }
}

//*********************************Loading section*******************************//

//checking if file existts
int isFileExists(const char *path)
{
    // try to open file
    FILE *fp = fopen(path, "rb");
    int status = 0;
    // if file does not exists
    if (fp != NULL)
    {
        status = 1;
        // file exists hence close file
        fclose(fp);
    }
    return status;
}

//checks if binary file exists in the system if not then create one with predifined password 
void init()
{
    FILE *fp = NULL;
    int status = 0;                         //to check status
    const char defaultUsername[] ="nits\n"; //Password info
    const char defaultPassword[] ="12345\n";
    sFileHeader fileHeaderInfo = {0};
    status = isFileExists(PATIENT_FILE);   //check if file exists
    if(!status)
    {
        //create file
        fp = fopen(PATIENT_FILE,"wb");
        if(fp != NULL)
        {
            //copy default password
            strncpy(fileHeaderInfo.password, defaultPassword, sizeof(defaultPassword));
            strncpy(fileHeaderInfo.username, defaultUsername, sizeof(defaultUsername));
            fwrite(&fileHeaderInfo, FILE_HEADER_SIZE, 1, fp);
            fclose(fp);
        }
    }
}

//Updates room database by checking occupied rooms from the file during loading
void initRoom(int roomNo, int id){
    roomLoaded[roomNo - 89].patientsId = id;
    roomLoaded[roomNo - 89].occupied = 1;
}

//creates a copy of database from the file into memory
void loadDatabase()
{
    patientCount = 0;
    char patientName[MAX_PATIENT_NAME] = {0};
    patient addPatientInfoFromDataBase = {0};
    FILE *fp = NULL;
    int status = 0;
    unsigned int countPatient = 1;
    fp = fopen(PATIENT_FILE,"rb");
    if(fp == NULL)
    {
        printf("File is not opened\n");
        exit(1);
    }
    if (fseek(fp,FILE_HEADER_SIZE,SEEK_SET) != 0)
    {
        fclose(fp);
        printf("Facing issue while reading file\n");
        exit(1);
    }
    int i = 0;
    while (fread (&addPatientInfoFromDataBase, sizeof(addPatientInfoFromDataBase), 1, fp))
    {
        //add patient data into memory one at a time
        patientLoaded[i] = addPatientInfoFromDataBase;
        i++;
        //update room database info 
        initRoom(addPatientInfoFromDataBase.roomNo, addPatientInfoFromDataBase.patientsId);
        patientCount++;//update total patient count
    }
    fclose(fp);
}

//function to display current task in header section
void printMessageCenter(const char* message)
{
    int len =0;
    int pos = 0;
    //calculate how many space need to print
    len = (78 - strlen(message))/2;
    printf("\t\t\t");
    for(pos=0 ; pos<len ; pos++)
    {
        //print space
        printf(" ");
    }
    //print message
    printf("%s",message);
}

//function to display header message
void headMessage(const char *message)
{
    system("cls");
    printf("\t\t\t#############################################################################");
    printf("\n\t\t\t############         NIT Silchar Quarentine Facility           ############");
    printf("\n\t\t\t###########################################################################");
    printf("\n\t\t\t---------------------------------------------------------------------------\n");
    printMessageCenter(message);
    printf("\n\t\t\t----------------------------------------------------------------------------");
}

//welcome screen message function
void welcomeMessage()
{
    headMessage("NIT SILCHAR");
    printf("\n\n\n\n\n");
    printf("\n\t\t\t  **-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**\n");
    printf("\n\t\t\t            =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=             ");
    printf("\n\t\t\t            =                 WELCOME                   =             ");
    printf("\n\t\t\t            =                   TO                      =             ");
    printf("\n\t\t\t            =              NIT Silchar                  =             ");
    printf("\n\t\t\t            =          Quarentine Facility              =             ");
    printf("\n\t\t\t            =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=             ");
    printf("\n\t\t\t  **-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**\n");
    printf("\n\n\n\t\t\t Enter any key to continue.....");
    getch();
}


//*******************Programming on loaded section*********************

//to check for valid name
int isNameValid(const char *name)
{
    int validName = 1;
    int len = 0;
    int index = 0;
    len = strlen(name);
    for(index =0; index <len ; ++index)
    {
        if(!(isalpha(name[index])) && (name[index] != '\n') && (name[index] != ' '))
        {
            validName = 0;
            break;
        }
    }
    return validName;
}

//function to check leap year, returns 1 if leap year
int IsLeapYear(int year)
{
    return (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0));
}
//to check if given date is valid
int isValidDate(Date *validDate)
{
    //check range of year,month and day
    if (validDate->yyyy > MAX_YR || validDate->yyyy < MIN_YR)
        return 0;
    if (validDate->mm < 1 || validDate->mm > 12)
        return 0;
    if (validDate->dd < 1 || validDate->dd > 31)
        return 0;
    //handle feb days in leap year
    if (validDate->mm == 2)
    {
        if (IsLeapYear(validDate->yyyy))
            return (validDate->dd <= 29);
        else
            return (validDate->dd <= 28);
    }
    //handle months which has only 30 days
    if (validDate->mm == 4 || validDate->mm == 6 || validDate->mm == 9 || validDate->mm == 11)
        return (validDate->dd <= 30);
    return 1;
}

//to find detalis of all rooms
void roomInfo()
{
    headMessage("All Room Details");
	for (int i = 0; i < TOTAL_ROOMS; i++)
	{
		if (roomLoaded[i].occupied)
		{
			printf("\n\t\t\tRoom number %d occupied\n", i+89);
			printf("\t\t\tRoom alloted to Patient ID %d\n", roomLoaded[i].patientsId);
		}
		else
		{
			printf("\n\t\t\tRoom number %d is vacant\n", i+89);
		}
	}
	printf("\n\n\t\t\tPress any key to go to main menu.....");
    fflush(stdin);
    getchar();
}

//to find all vacant rooms
void vacantRoomInfo()
{
    headMessage("Vancant Rooms");
	printf("\n\t\t\tFollowing are the vacent rooms\n\n\n");
	for (int i = 0; i < TOTAL_ROOMS; i++)
	{
		if(!roomLoaded[i].occupied)
		{
			printf("\t\t\tRoom number %d\n\n", i+89);
		}
	}
	printf("\n\n\t\t\tPress any key to go to main menu.....");
    fflush(stdin);
    getchar();
}

//to find all occupied rooms
void occupiedRoomInfo()
{
    headMessage("Occupied Rooms");
	printf("\n\t\t\tFollowing are the occupied rooms\n\n");
	for (int i = 0; i < TOTAL_ROOMS; i++)
	{
		if(roomLoaded[i].occupied)
		{
			printf("\n\t\t\tRoom number %d\n", i+89);
			printf("\t\t\tRoom alloted to Patient ID %d\n", roomLoaded[i].patientsId);
		}
	}
	printf("\n\n\t\t\tPress any key to go to main menu.....");
    fflush(stdin);
    getchar();
}

//merge sort helper function
void Merge(patient a[],int lb,int mid,int ub)
{
    int i, j, k;
    int n1 = mid-lb+1;
    int n2 = ub-mid;
    patient l[n1], r[n2];
    for(i=0; i<n1; i++){
        l[i] = a[lb+i];
    }
    for(j=0; j<n2; j++){
        r[j] = a[mid+1+j];
    }
    i=0; j=0; k=lb;
    while(i<n1 && j<n2){
        if(l[i].roomNo <= r[j].roomNo){
            a[k] = l[i];
            i++;
        }
        else{
            a[k] = r[j];
            j++;
        }
        k++;
    }
    while(i<n1){
        a[k] = l[i];
        i++; k++;
    }
    while(j<n2){
        a[k] = r[j];
        j++; k++;
    }
}
//mergesort function
void mergeSort(patient a[],int lb,int ub){
    if(lb<ub){
        int mid = (lb+ub)/2;
        mergeSort(a,lb,mid);
        mergeSort(a,mid+1,ub);
        Merge(a,lb,mid,ub);
    }
}

//to sort copy of database in memory according to rrom number
void sortRoomNumberWise()
{
    headMessage("Sorting Patients Room number wise");
    mergeSort(patientLoaded, 0, patientCount-1); //sort patients using mersort function
    printf("\n\n\t\t\tSuccessfully sorted the patients room number wise\n\n\n");
    printf("\n\n\t\t\tPress any key to go to main menu.....");
    fflush(stdin);
    getchar();
}

//allot room number to new patients if any room is vacant
int allotRoom(int id){
    for(int i=0; i<TOTAL_ROOMS; i++){
        if((!roomLoaded[i].occupied)){
            roomLoaded[i].patientsId = id;
            roomLoaded[i].occupied = 1;
            return (i+89);
        }
    }
    printf("Sorry, all rooms are occupied\n");
    return 0;
}

//add patients in database as well as in the memory array
void addPatientInDataBase()
{
    int idCheck; //temporary variable to check for available room
    int days;
    patient addPatientInfoInDataBase = {0};
    FILE *fp = NULL;
    int status = 0;
    fp = fopen(PATIENT_FILE,"ab+");
    if(fp == NULL)
    {
        printf("File is not opened\n");
        exit(1);
    }
    headMessage("ADD NEW PATIENTS");
    printf("\n\n\t\t\tENTER YOUR DETAILS BELOW:");
    printf("\n\t\t\t---------------------------------------------------------------------------\n");
    printf("\n\t\t\tPatient ID NO  = ");
    fflush(stdin);
    scanf("%u",&idCheck);
    addPatientInfoInDataBase.roomNo = allotRoom(idCheck);
    if(addPatientInfoInDataBase.roomNo == 0) //if room is full return
    {
        printf("Sorrry, all rooms are occupied\n");
        return;
    }
    addPatientInfoInDataBase.patientsId = idCheck;
    
    do//get the name of patient untill it is valid
    {
        printf("\n\t\t\tPatient Name  = ");
        fflush(stdin);
        fgets(addPatientInfoInDataBase.patientName,MAX_PATIENT_NAME,stdin);
        status = isNameValid(addPatientInfoInDataBase.patientName);
        if (!status)
        {
            printf("\n\t\t\tName contain invalid character. Please enter again.");
        }
    }
    while(!status);

    printf("\n\t\t\tPatient Age  = ");
    fflush(stdin);
    scanf("%d", &addPatientInfoInDataBase.patientAge);

    printf("\n\t\t\tPatient Address  = ");
    fflush(stdin);
    fgets(addPatientInfoInDataBase.patientAddr,MAX_ADDRESS,stdin);
        
    printf("\n\t\t\tPatient coming from  = ");
    fflush(stdin);
    fgets(addPatientInfoInDataBase.patientComingFrom,MAX_ADDRESS,stdin);
        
    printf("\n\t\t\tPatient going to  = ");
    fflush(stdin);
    fgets(addPatientInfoInDataBase.patientGoingTo,MAX_ADDRESS,stdin);

    do//get date year,month and day untill it is valid
    {
        printf("\n\t\t\tEnter admit date in format (day/month/year): ");
        scanf("%d/%d/%d",&addPatientInfoInDataBase.patientAdmitDate.dd,&addPatientInfoInDataBase.patientAdmitDate.mm,
            &addPatientInfoInDataBase.patientAdmitDate.yyyy);
        status = isValidDate(&addPatientInfoInDataBase.patientAdmitDate);
        if (!status)
        {
            printf("\n\t\t\tPlease enter a valid date.\n");
        }
    }
    while(!status);
    
    do//get date year,month and day untill it is valid
    {
        printf("\n\t\t\tEnter discharge date in format (day/month/year): ");
        scanf("%d/%d/%d",&addPatientInfoInDataBase.patientDischargeDate.dd,&addPatientInfoInDataBase.patientDischargeDate.mm,
            &addPatientInfoInDataBase.patientDischargeDate.yyyy);
        status = isValidDate(&addPatientInfoInDataBase.patientDischargeDate);
        if (!status)
        {
            printf("\n\t\t\tPlease enter a valid date.\n");
        }
    }
    while(!status);

    fwrite(&addPatientInfoInDataBase,sizeof(addPatientInfoInDataBase), 1, fp); //write data into file
    fclose(fp);
}


//to search patients by their name
void merger(patient a[],int lb,int mid,int ub)
{
    int i,j,k;
    int n1 = mid-lb+1;
    int n2 = ub-mid;
    patient l[n1],r[n2];
    for(i=0;i<n1;i++){
        l[i]=a[lb+i];
    }
    for(j=0;j<n2;j++){
        r[j]=a[mid+1+j];
    }
    i=0;j=0;k=lb;
    while(i<n1 && j<n2){
        if(strcmp(l[i].patientName, r[j].patientName) <= 0){
            a[k] = l[i];
            i++;
        }
        else{
            a[k] = r[j];
            j++;
        }
        k++;
    }
    while(i<n1){
        a[k] = l[i];
        i++;k++;
    }
    while(j<n2){
        a[k] = r[j];
        j++;k++;
    }
}
//mergesort function
void mergeSortString(patient a[],int lb,int ub){
    if(lb<ub){
        int mid = (lb+ub)/2;
        mergeSortString(a,lb,mid);
        mergeSortString(a,mid+1,ub);
        merger(a,lb,mid,ub);
    }
}
int binarySearchString(patient *a,int l,int r,char* name){
    while(l<=r)
    {
    	int m =l + (r-1)/2;
    	int res = strcmp(a[m].patientName ,name);
    	if(!res)
    		return m;//so that 0th index is not ignored

    	if(res < 0)
    		l = m+1;
    	else
    		r = m-1;
    }
    return -1;
}

void searchPatientsByName()
{
	headMessage("SEARCH PATIENTS BY NAME");
    char patientName[MAX_PATIENT_NAME];
    printf("\n\n\t\t\tEnter patient's name to search:");
    fflush(stdin);
    fgets(patientName,MAX_PATIENT_NAME,stdin);
    mergeSortString(patientLoaded, 0, patientCount-1);
    int found = binarySearchString(patientLoaded, 0, patientCount-1, patientName);
    if(found != -1)
    {
        printf("\n\t\t\tPatient id = %u\n",patientLoaded[found].patientsId);
        printf("\t\t\tPatient name = %s",patientLoaded[found].patientName);
        printf("\t\t\tRoom number alloted = %d\n",patientLoaded[found].roomNo);
        printf("\t\t\tPatient age = %d",patientLoaded[found].patientAge);
        printf("\n\t\t\tPatient Address = %s",patientLoaded[found].patientAddr);
        printf("\t\t\tPatient coming from = %s",patientLoaded[found].patientComingFrom);
        printf("\t\t\tPatient going to = %s",patientLoaded[found].patientGoingTo);
        printf("\t\t\tPatient admit date(day/month/year) =  (%d/%d/%d)",patientLoaded[found].patientAdmitDate.dd,
               patientLoaded[found].patientAdmitDate.mm, patientLoaded[found].patientAdmitDate.yyyy);
        printf("\n\t\t\tPatient discharge date(day/month/year) =  (%d/%d/%d)",patientLoaded[found].patientDischargeDate.dd,
               patientLoaded[found].patientDischargeDate.mm, patientLoaded[found].patientDischargeDate.yyyy);
    }
    else
    {
        printf("\n\t\t\tNo Record");
    }
    printf("\n\n\n\t\t\tPress any key to go to main menu.....");
    fflush(stdin);
    getchar();
}

//search a patient by room number
int binarySearch(patient *a,int l,int r,int room){
    while(l<=r)
    {
    	int m =l + (r-1)/2;
    	if(a[m].roomNo == room)
    		return m;//so that 0th index is not ignored

    	if(a[m].roomNo < room)
    		l = m+1;
    	else
    		r = m-1;
    }
    return -1;
}

void searchPatientsByRoomNo()
{
    headMessage("SEARCH PATIENTS BY ROOM NUMBER");
    int room;
    printf("\n\n\t\t\tEnter Room number to search:");
    fflush(stdin);
    scanf("%d", &room);
    mergeSort(patientLoaded, 0, patientCount-1);
    int found = binarySearch(patientLoaded, 0, patientCount-1, room);
    if(found != -1)
    {
        printf("\n\t\t\tPatient id = %u\n",patientLoaded[found].patientsId);
        printf("\t\t\tPatient name = %s",patientLoaded[found].patientName);
        printf("\t\t\tRoom number alloted = %d\n",patientLoaded[found].roomNo);
        printf("\t\t\tPatient age = %d",patientLoaded[found].patientAge);
        printf("\n\t\t\tPatient Address = %s",patientLoaded[found].patientAddr);
        printf("\t\t\tPatient coming from = %s",patientLoaded[found].patientComingFrom);
        printf("\t\t\tPatient going to = %s",patientLoaded[found].patientGoingTo);
        printf("\t\t\tPatient admit date(day/month/year) =  (%d/%d/%d)",patientLoaded[found].patientAdmitDate.dd,
               patientLoaded[found].patientAdmitDate.mm, patientLoaded[found].patientAdmitDate.yyyy);
        printf("\n\t\t\tPatient discharge date(day/month/year) =  (%d/%d/%d)",patientLoaded[found].patientDischargeDate.dd,
               patientLoaded[found].patientDischargeDate.mm, patientLoaded[found].patientDischargeDate.yyyy);
    }
    else
    {
        printf("\n\t\t\tNo Record");
    }
    printf("\n\n\n\t\t\tPress any key to go to main menu.....");
    fflush(stdin);
    getchar();
}

//to display all the entries
void viewPatients()
{
    headMessage("All Admitted Patients");
    if(patientCount == 0)
    {
        printf("\n\n\t\t\tNo entries found\n");
        printf("\n\n\t\t\tPress any key to go to main menu.....");
        fflush(stdin);
        getchar();
    }
    for(int i=0; i< patientCount; i++)
    {
        printf("\n\n\t\t\tEntry number = %d\n", i+1);
        printf("\n\t\t\tPatient id = %u\n", patientLoaded[i].patientsId);
        printf("\t\t\tPatient name = %s", patientLoaded[i].patientName);
        printf("\t\t\tRoom number alloted = %d\n", patientLoaded[i].roomNo);
        printf("\t\t\tPatient age = %d", patientLoaded[i].patientAge);
        printf("\n\t\t\tPatient Address = %s", patientLoaded[i].patientAddr);
        printf("\t\t\tPatient coming from = %s", patientLoaded[i].patientComingFrom);
        printf("\t\t\tPatient going to = %s", patientLoaded[i].patientGoingTo);
        printf("\t\t\tPatient admit date(day/month/year) =  (%d/%d/%d)", patientLoaded[i].patientAdmitDate.dd,
               patientLoaded[i].patientAdmitDate.mm, patientLoaded[i].patientAdmitDate.yyyy);
        printf("\n\t\t\tPatient discharge date(day/month/year) =  (%d/%d/%d)", patientLoaded[i].patientDischargeDate.dd,
               patientLoaded[i].patientDischargeDate.mm, patientLoaded[i].patientDischargeDate.yyyy);

    }
    printf("\n\n\t\t\tPress any key to go to main menu.....");
    fflush(stdin);
    getchar();
}


//to discharge a patient
//copy every patient data to a temporary file except the one to be discharged 
//and replaces the original file with the temporary file
void deletePatients()
{
    int found = 0;
    int patientDelete = 0;
    sFileHeader fileHeaderInfo = {0};
    char patientName[MAX_PATIENT_NAME] = {0};
    patient addPatientInfoInDataBase = {0};
    FILE *fp = NULL;
    FILE *tmpFp = NULL;
    int status = 0;
    headMessage("Delete Patients Details");
    fp = fopen(PATIENT_FILE,"rb");
    if(fp == NULL)
    {
        printf("File is not opened\n");
        exit(1);
    }
    tmpFp = fopen("tmp.bin","wb");
    if(tmpFp == NULL)
    {
        fclose(fp);
        printf("File is not opened\n");
        exit(1);
    }
    fread (&fileHeaderInfo,FILE_HEADER_SIZE, 1, fp);
    fwrite(&fileHeaderInfo,FILE_HEADER_SIZE, 1, tmpFp);
    printf("\n\t\t\tEnter Patient ID NO. for delete:");
    scanf("%d",&patientDelete);
    while (fread (&addPatientInfoInDataBase, sizeof(addPatientInfoInDataBase), 1, fp))
    {
        if(addPatientInfoInDataBase.patientsId != patientDelete)
        {
            fwrite(&addPatientInfoInDataBase,sizeof(addPatientInfoInDataBase), 1, tmpFp);
        }
        else
        {
            //delete room entry too
            roomLoaded[addPatientInfoInDataBase.roomNo - 89].occupied = 0;
            found = 1;
        }
    }
    (found)? printf("\n\t\t\tRecord deleted successfully....."):printf("\n\t\t\tRecord not found");
    fclose(fp);
    fclose(tmpFp);
    remove(PATIENT_FILE);
    rename("tmp.bin",PATIENT_FILE);
    printf("\n\n\t\t\tPress any key to go to main menu.....");
    fflush(stdin);
    getchar();
}

//to update password
void updateCredential(void)
{
    sFileHeader fileHeaderInfo = {0};
    FILE *fp = NULL;
    unsigned char userName[MAX_SIZE_USER_NAME] = {0};
    unsigned char password[MAX_SIZE_PASSWORD] = {0};
    headMessage("Update Credential");
    fp = fopen(PATIENT_FILE,"rb+");
    if(fp == NULL)
    {
        printf("File is not opened\n");
        exit(1);
    }
    fread (&fileHeaderInfo,FILE_HEADER_SIZE, 1, fp);
    if (fseek(fp,0,SEEK_SET) != 0)
    {
        fclose(fp);
        printf("\n\t\t\tFacing issue while updating password\n");
        exit(1);
    }
    printf("\n\n\t\t\tNew Username:");
    fflush(stdin);
    fgets(userName,MAX_SIZE_USER_NAME,stdin);
    printf("\n\n\t\t\tNew Password:");
    fflush(stdin);
    fgets(password,MAX_SIZE_PASSWORD,stdin);
    strncpy(fileHeaderInfo.username,userName,sizeof(userName));
    strncpy(fileHeaderInfo.password,password,sizeof(password));
    fwrite(&fileHeaderInfo,FILE_HEADER_SIZE, 1, fp);
    fclose(fp);
    printf("\n\t\t\tYour Password has been changed successfully");
    printf("\n\t\t\ttLogin Again:");
    fflush(stdin);
    getchar();
    exit(1);
}

//Left part of loading section
void menu()
{
    int choice = 0;
    while(1) //display menu infinitely until exited
    {
        headMessage("MAIN MENU");
        printf("\n\n\n\t\t\t1.Add patients");
        printf("\n\t\t\t2.Search patients by name");
        printf("\n\t\t\t3.Search patients by room number");
        printf("\n\t\t\t4.View all patients");
        printf("\n\t\t\t5.Discharge a patient");
        printf("\n\t\t\t6.Check all rooms");
        printf("\n\t\t\t7.Check vacant rooms");
        printf("\n\t\t\t8.Check occupied rooms");
        printf("\n\t\t\t9.Sort patients room number wise");
        printf("\n\t\t\t10.Update password");
        printf("\n\t\t\t0.Exit");
        printf("\n\n\n\t\t\tEnter choice => ");
        scanf("%d",&choice);
        switch(choice)
        {
        case 1:
            addPatientInDataBase();
            loadDatabase();// update the patients database with new entries from file
            break;
        case 2:
            searchPatientsByName();
            break;
        case 3:
            searchPatientsByRoomNo();
            break;
        case 4:
            viewPatients();
            break;
        case 5:
            deletePatients();
            loadDatabase();// update the patients database with updted entries from file
            break;
        case 6:
            roomInfo();
            break;
        case 7:
            vacantRoomInfo();
            break;
        case 8:
            occupiedRoomInfo();
            break;
        case 9:
            sortRoomNumberWise();
            break;
        case 10:
            updateCredential();
            break;
        case 0:
            exit(1);
            break;
        default:
            printf("\n\n\n\t\t\tINVALID INPUT!!! Try again...");
            printf("\n\n\t\t\tPress any key to go to main menu.....");
	    	fflush(stdin);
	    	getchar();
        }
    }
}

//to check login password during booting of the program
void login()
{
    unsigned char userName[MAX_SIZE_USER_NAME] = {0};
    unsigned char password[MAX_SIZE_PASSWORD] = {0};
    int L=0;
    sFileHeader fileHeaderInfo = {0};
    FILE *fp = NULL;
    headMessage("Login");
    fp = fopen(PATIENT_FILE,"rb");
    if(fp == NULL)
    {
        printf("File is not opened\n");
        exit(1);
    }
    fread(&fileHeaderInfo,FILE_HEADER_SIZE, 1, fp);
    fclose(fp);
    do
    {
        printf("\n\n\n\t\t\t\tUsername:");
        fgets(userName,MAX_SIZE_USER_NAME,stdin);
        printf("\n\t\t\t\tPassword:");
        fgets(password,MAX_SIZE_PASSWORD,stdin);
        if((!strcmp(userName,fileHeaderInfo.username)) && (!strcmp(password,fileHeaderInfo.password)))
        {
            menu();//f password matched cater the menu section
        }
        else
        {
            printf("\t\t\t\tLogin Failed Enter Again Username & Password\n\n");
            L++;
        }
    }
    //to exit the program if more than three unsuccessful attempts are made
    while(L<=3);
    if(L>3)
    {
        headMessage("Login Failed");
        printf("\t\t\t\tSorry,Unknown User.");
        getch();
        system("cls");
    }
}

int main()
{
    init(); //initialises files
    roomInitialization(); //initialises room data
    loadDatabase(); //loads the patients data from files to memory
    welcomeMessage(); //
    login(); //checks password
    return 0;
}