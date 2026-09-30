#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#define MIN_SEVERITY 1
#define MAX_SEVERITY 5

typedef struct node{
    int ID;
    char name[101];
    int severity;
    struct node* next;

}Node;

typedef struct LinkedList{
    Node* head;
} Queue;

Node* createNode(int nodeID, char nodeName[101], int nodeSeverity){
    if(nodeName == NULL || strlen(nodeName) == 0 || strlen(nodeName) > 100){
        return NULL;
    }
    Node* newNode = (Node*)malloc(sizeof(Node));
    if(newNode != NULL){
        newNode->ID = nodeID;
        int i = 0;
         while(nodeName[i] != '\0'){
            newNode->name[i] = nodeName[i];
            i++;
         }
         newNode->name[i] = '\0';
        newNode->severity = nodeSeverity;
        newNode->next = NULL;
    }
    return newNode;
    
}

bool addPatient(int nodeID, char nodeName[101], int nodeSeverity, Queue *list){
    if(nodeID <= 0 || nodeSeverity < MIN_SEVERITY || nodeSeverity > MAX_SEVERITY
       || nodeName == NULL || strlen(nodeName) == 0 || strlen(nodeName) > 100){
        printf("Error: Use a positive ID, a name of 1-100 characters, and severity 1-5.\n");
        return false;
    }
    Node* current = list->head;
    if(current == NULL){
        // the list is empty so adds patient at the front of the queue
        Node *temp = createNode(nodeID, nodeName, nodeSeverity);
        if(temp == NULL){
            printf("Error: Could not allocate memory for patient.\n");
            return false;
        }
        temp->next = list->head;
        list->head = temp;
        printf("Patient %d Added.\n", list->head->ID);
        return true;
    }
    //checks if the patient ID isn't part of the list already
    while(current->ID != nodeID && current->next != NULL){
        current = current->next;
    }
    if(current->ID == nodeID){
        printf("Error: Patient %d already exists.\n", current->ID);
        return false;

    }else{//adds new patient to queue depending on severity and arrival
        current = list->head;
        if(current->severity < nodeSeverity){
            Node *newNode = createNode(nodeID, nodeName, nodeSeverity);
            if(newNode == NULL){
                printf("Error: Could not allocate memory for patient.\n");
                return false;
            }
            
            newNode->next = list->head;
            list->head = newNode;
            printf("Patient %d Added.\n", list->head->ID);
            return true;
        }
        while(current->next != NULL && current->next->severity >= nodeSeverity){
            current = current->next;
        }
        Node *newNode = createNode(nodeID, nodeName, nodeSeverity);
        if(newNode == NULL){
            printf("Error: Could not allocate memory for patient.\n");
            return false;
        }

        //link to the rest of list;
        newNode->next = current->next;
        current->next = newNode; // overWrite next with the newNode.
        printf("Patient %d Added.\n", current->next->ID);
        return true;

    }

    
}
bool treatPatient(Queue *list){
    if(list->head == NULL){
        printf("Queue is empty.\n");
        return false;
    }
    Node* newHead = list->head->next;
    printf("Patient %d Treated.\n", list->head->ID);
    free(list->head);
    list->head = newHead;
    return true;
}

bool removePatient(int nodeID, Queue *list){
    if(nodeID <= 0){
        printf("Error: Patient ID must be positive.\n");
        return false;
    }
    if(list->head == NULL){
        //There is nothing to remove
        printf("Error: Patient %d not found.\n", nodeID);
        return false;
    }
    //The first patient matches the ID of the patient to  be removed
    if(list->head->ID == nodeID){
        Node *newHead = list->head->next;
        printf("Patient %d Removed.\n",list->head->ID);
        free(list->head);
        list->head = newHead;
        return true;
    }
    //search for patient that matches the patient ID to be removed
    Node *current = list->head;
    while(current->next != NULL && current->next->ID != nodeID){
        current = current->next;
    }
    if(current->next != NULL){
        printf("Patient %d Removed.\n",current->next->ID);
        Node *temp = current->next;
        current->next = temp->next;
        free(temp);
        return true;
    }
    printf("Error: Patient %d not found.\n",nodeID);
    return false;
}

void displayQueue(Queue *list){
    if(list->head == NULL){
        printf("Queue is empty.\n");
        return;
    }else if(list->head->next == NULL){
        printf("ID: %d | Name: %s | Severity: %d\n",list->head->ID, list->head->name, list->head->severity);
    }else{
    Node *current = list->head;
    while(current->next != NULL){
        printf("ID: %d | Name: %s | Severity: %d\n",current->ID, current->name, current->severity);
        current = current->next;
    }
    printf("ID: %d | Name: %s | Severity: %d\n",current->ID, current->name, current->severity);
    return;
    }
}
void endProgram(Queue *list){
    while(list->head != NULL){
        Node *newHead = list->head->next;
        free(list->head);
        list->head = newHead;
    }
    list->head = NULL;
    return ;
}

// Parse a whole integer token, rejecting overflow and trailing characters.
bool parseInteger(const char *text, int *value){
    char *end;
    errno = 0;
    long parsed = strtol(text, &end, 10);
    if(text == end || *end != '\0' || errno == ERANGE
       || parsed < INT_MIN || parsed > INT_MAX){
        return false;
    }
    *value = (int)parsed;
    return true;
}

int main(void){
    char line[512];
    Queue list;
    list.head = NULL;
    while(fgets(line, sizeof(line), stdin) != NULL){
        // Discard an overlong command completely before accepting another one.
        if(strchr(line, '\n') == NULL && !feof(stdin)){
            int ch;
            while((ch = getchar()) != '\n' && ch != EOF){}
            printf("Error: Command is too long.\n");
            continue;
        }
        char *tokens[5];
        int count = 0;
        char *token = strtok(line, " \t\r\n");
        while(token != NULL && count < 5){
            tokens[count++] = token;
            token = strtok(NULL, " \t\r\n");
        }
        if(count == 0){
            continue;
        }
        int ID, severity;
        if(strcmp(tokens[0], "A") == 0 && count == 4){
            if(!parseInteger(tokens[1], &ID) || !parseInteger(tokens[3], &severity)){
                printf("Error: ID and severity must be valid integers.\n");
                continue;
            }
            addPatient(ID, tokens[2], severity, &list);
        }else if(strcmp(tokens[0], "T") == 0 && count == 1){
            treatPatient(&list);
        }else if(strcmp(tokens[0], "R") == 0 && count == 2){
            if(!parseInteger(tokens[1], &ID)){
                printf("Error: ID must be a valid integer.\n");
                continue;
            }
            removePatient(ID,&list);
        }else if(strcmp(tokens[0], "D") == 0 && count == 1){
            displayQueue(&list);
        }else if(strcmp(tokens[0], "Q") == 0 && count == 1){
            break;
        }else{
            printf("Error: Use A <ID> <name> <severity>, T, R <ID>, D, or Q.\n");
        }
    }
    bool inputFailed = ferror(stdin) != 0;
    if(inputFailed){
        fprintf(stderr, "Error: Could not read input.\n");
    }
    endProgram(&list);
    return inputFailed ? EXIT_FAILURE : EXIT_SUCCESS;
    

}
