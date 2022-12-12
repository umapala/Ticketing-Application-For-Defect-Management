#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SUCCESS 1
#define ERROR 0
#define MAXFILES 5 				// Maximum Input Defect files is set to 5
#define MAXEMP 10
#define MAXSTRLEN 200
#define MAXDEFECT 10

struct defect
{
    char *defectID;
    char *description;
    char *moduleName;
    char *functionalArea;
    char *date;
    char *status;
    char *type;
};
typedef struct defect defect;
struct employee
{
    char *Id;
    char *Name;
    char *BUnit;
    char *Expertise;
    char *Designation;
    int n_defect;
    defect *assigned_arr[MAXDEFECT];
};
typedef struct employee Emp;

Emp *emp_arr[MAXEMP]; 				// Array of Employee Structure
int n_emp; 					// Total count of Employee


int getEmployee(Emp *arr[]);
void *getDefect(void *file);
int checkValidity(char *str);
void validDefect(defect *defectptr, char *str);
void invalidDefect(char *str);
void assignEmployee(defect *arr[], int vdc);
void searchProgrammer(defect *defectptr, Emp *arr[], int n_emp);
void createEmployeeFile(Emp *emp_ptr, defect *defectptr);
void unassignedDefect(defect *defectptr);






int getEmployee(Emp *arr[])
{
    char *f_loc = "employees.txt"; 	// Location of Employee Database

    FILE *fpr = fopen(f_loc, "r");
    if (fpr == NULL)
    {
        printf("\n--- Unable to read Employee file at %s ---\n", f_loc);
        exit(1);
    }

    char str[MAXSTRLEN];
    int i = 0;
    while (1)
    {
        if (fgets(str, MAXSTRLEN, fpr) == NULL)
        {
            break;
        }

        arr[i] = (Emp *)calloc(1, sizeof(Emp));

        char *newstr = (char *)calloc(strlen(str), sizeof(char));
        strcpy(newstr, str);
        char *token = strtok(newstr, ":");
        arr[i]->Id = (char *)calloc(strlen(token), sizeof(char));
        arr[i]->Id = token;
        token = strtok(NULL, ":");
        arr[i]->Name = (char *)calloc(strlen(token), sizeof(char));
        arr[i]->Name = token;
        token = strtok(NULL, ":");
        arr[i]->BUnit = (char *)calloc(strlen(token), sizeof(char));
        arr[i]->BUnit = token;
        token = strtok(NULL, ":");
        arr[i]->Expertise = (char *)calloc(strlen(token), sizeof(char));
        arr[i]->Expertise = token;
        token = strtok(NULL, ":");
        arr[i]->Designation = (char *)calloc(strlen(token), sizeof(char));
        arr[i]->Designation = token;
        arr[i]->n_defect = 0;
        arr[i]->assigned_arr[MAXDEFECT] = (defect *)calloc(5, sizeof(defect));
        i++;
    }
    return i;
}



void *getDefect(void *file)
{
    defect *defect_arr[MAXDEFECT];
    char *file_loc = (char *)file;

    FILE *fpr = fopen(file_loc, "r");
    if (fpr == NULL) // File Validation
    {
        printf("\n--- Cannot open file: %s\n", file_loc);
        exit(1);
    }
    printf("\n--- Processing file: %s", file_loc);

    							// Reading from defect file
    char str[MAXSTRLEN];
    int vdc = 0;
    while (1)
    {
        if (fgets(str, MAXSTRLEN, fpr) == NULL)
        {
            break;
        }
        if (checkValidity(str))
        {
            defect_arr[vdc] = (defect *)calloc(1, sizeof(defect));
            validDefect(defect_arr[vdc], str);
            vdc++;
        }
        else
        {
            invalidDefect(str);
        }
    }
    fclose(fpr);
    
    assignEmployee(defect_arr, vdc);

    for (int i = 0; i < vdc; i++)
    {
        free(defect_arr[i]);
    }
    
}



int checkValidity(char *str)
{
    int count = 0;
    char *s = (char *)calloc(strlen(str), sizeof(char));
    strcpy(s, str);

    char *token = strtok(s, ":");
    while (token != NULL)
    {
        count++;
        token = strtok(NULL, ":");
    }
    if (count == 7)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}



void validDefect(defect *defectptr, char *str)
{
    char *newstr = (char *)calloc(strlen(str), sizeof(char));
    strcpy(newstr, str);
    char *token = strtok(newstr, ":");
    defectptr->defectID = (char *)calloc(strlen(token), sizeof(char));
    defectptr->defectID = token;
    token = strtok(NULL, ":");
    defectptr->description = (char *)calloc(strlen(token), sizeof(char));
    defectptr->description = token;
    token = strtok(NULL, ":");
    defectptr->moduleName = (char *)calloc(strlen(token), sizeof(char));
    defectptr->moduleName = token;
    token = strtok(NULL, ":");
    defectptr->functionalArea = (char *)calloc(strlen(token), sizeof(char));
    defectptr->functionalArea = token;
    token = strtok(NULL, ":");
    defectptr->date = (char *)calloc(strlen(token), sizeof(char));
    defectptr->date = token;
    token = strtok(NULL, ":");
    defectptr->status = (char *)calloc(strlen(token), sizeof(char));
    defectptr->status = token;
    token = strtok(NULL, ":");
    defectptr->type = (char *)calloc(strlen(token), sizeof(char));
    defectptr->type = token;
}


void invalidDefect(char *str)
{
    char *newstr = (char *)calloc(strlen(str), sizeof(char));
    strcpy(newstr, str);
    char *token = strtok(newstr, ":");
    printf("\nDefect ID: %s contains insufficient information.", token);
    char *filename = "output/invalidDefect.txt";
    FILE *fp;
    fp = fopen(filename, "a");
    if (fp == NULL)
    {
        printf("\n--- Unable to write into output file ---\n");
        exit(1);
    }
    fprintf(fp, "%s", str);
    fclose(fp);
}



void assignEmployee(defect *arr[], int vdc)
{

    int odc = 0; 				// odc: Open Defects Count
    for (int i = 0; i < vdc; i++)
    {
        if (!strcmp(arr[i]->status, "open"))
        {
            odc++;
            searchProgrammer(arr[i], emp_arr, n_emp);
        }
    }
    
}



void searchProgrammer(defect *defectptr, Emp *arr[], int n_emp)
{
    int i, foundflag = 0;
    for (i = 0; i < n_emp; i++)
    {
        if (strcmp(defectptr->functionalArea, arr[i]->Expertise) == 0)
        {
            foundflag = 1;
            defectptr->status = "Assigned";
            arr[i]->n_defect++;
            arr[i]->assigned_arr[(arr[i]->n_defect) - 1] = defectptr;
            createEmployeeFile(arr[i], defectptr);
            printf("\nDefect ID: %s\nModule Name: %s\nFunctional Area: %s\nDescription: %s\nHas been assigned to:-\nEmployee Id: %s\nEmployee Name: %s\n",defectptr->defectID, defectptr->moduleName, defectptr->functionalArea, defectptr->description, arr[i]->Id, arr[i]->Name);
            break;
        }
    }
    if (foundflag == 0)
    {
        printf("\n--- Programmer not found for defect Id: %s ---", defectptr->defectID);
        unassignedDefect(defectptr);
        return;
    }
}



void createEmployeeFile(Emp *emp_ptr, defect *defectptr)
{
    char out_file[MAXSTRLEN];
    sprintf(out_file, "output/%s_assignments.txt", emp_ptr->Id);
    FILE *fp = fopen(out_file, "a");
    if (fp == NULL)
    {
        printf("\n--- Unable to write into output Employee file ---\n");
        exit(1);
    }
    fprintf(fp, "%s : %s : %s : %s : %s : %s : %s : %s", emp_ptr->Id, emp_ptr->Name, defectptr->defectID, defectptr->description, defectptr->moduleName, defectptr->functionalArea, defectptr->date, defectptr->type);
    fclose(fp);
}



void unassignedDefect(defect *defectptr)
{
    char *fileDefectPtr = "output/unassignedDefect.txt";
    FILE *flp;
    flp = fopen(fileDefectPtr, "a");
    if (flp == NULL)
    {
        printf("\n--- Cannot open %s file ---", fileDefectPtr);
        exit(1);
    }
    fprintf(flp, "%s : %s : %s : %s : %s : %s : %s", defectptr->defectID,
            defectptr->description,
            defectptr->moduleName,
            defectptr->functionalArea,
            defectptr->date,
            defectptr->status,
            defectptr->type);
    fclose(flp);
}



int main(int argc, char *argv[])
{
    int n_files = argc - 1; 				// Total Defect Files to be checked
    printf("--- Total files in queue: %d ---", n_files);
    if (argc < 2)
    {
        printf("--- Defect files not passed. ---");
        exit(1);
    }

    n_emp = getEmployee(emp_arr);

    for (int i = 1; i < argc; i++)
    {
	getDefect(argv[i]);	
    }
    return 0;	
}