#include "structures.h"

void addNewMedication(BTree* Trees) {
    MedNode* newMed = (MedNode*)malloc(sizeof(MedNode));
    if (!newMed) {
        printf("Memory allocation failed!\n");
        return;
    }
    printf("\nEnter Medication ID: ");
    if (scanf("%d", &newMed->medID) != 1) {
        printf("Invalid Medication ID.\n");
        free(newMed);
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n'); 
    printf("Enter Medication Name: ");
    fgets(newMed->medName, NAME_LEN, stdin);
    newMed->medName[strcspn(newMed->medName, "\n")] = '\0';

    if (searchIn_Med_Hash(newMed->medName) != -1) {
        printf("Medication name already exists!\n");
        free(newMed);
        return;
    }

    printf("Enter Price per Unit: ");
    if (scanf("%f", &newMed->pricePerUnit) != 1) {
        printf("Invalid price.\n");
        free(newMed);
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');
    printf("reorder Level: ");
    if (scanf("%f", &newMed->reorderLevel) != 1) {
        printf("Invalid price.\n");
        free(newMed);
        while (getchar() != '\n');
        return;
    }

    newMed->stockQuantity = 0;
    newMed->totalSales = 0;
    newMed->batches = NULL;
    newMed->suppliers = NULL;

    insertIn_Med_Hash(newMed->medName, newMed->medID);

    // --- Add Batches ---
    int bNo;
    printf("\nNumber of batches to add: ");
    if (scanf("%d", &bNo) != 1 || bNo < 0) {
        printf("Invalid number of batches.\n");
        free(newMed);
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n'); 

    memset (newMed->suppliers, NULL, MAX_MED_IN_SUPP);

    for (int i = 1; i <= bNo; i++) {
        BatchNode* newBatch = (BatchNode*)malloc(sizeof(BatchNode));
        if (!newBatch) {
            printf("Failed to allocate batch.\n");
            freeBatchList(newMed->batches); 
            free(newMed);
            return;
        }
        newBatch->next = NULL;
        newBatch->medPtr = newMed;

        printf("\nBatch %d:\n\tBatch number: ", i);
        fgets(newBatch->batchNo, NUM_LEN, stdin);
        newBatch->batchNo[strcspn(newBatch->batchNo, "\n")] = '\0';

        char date[DATE_LEN];
        printf("\tExpiration date (DD-MM-YYYY): ");
        fgets(date, DATE_LEN, stdin);
        date[strcspn(date, "\n")] = '\0';
        newBatch->expiryDate = check_and_correct_DATE(date);
        if (newBatch->expiryDate == -1) {
            printf("Invalid date.\n");
            free(newBatch);
            continue;
        }

        printf("\tStock quantity: ");
        if (scanf("%d", &newBatch->stock) != 1 || newBatch->stock < 1) {
            printf("Invalid stock value.\n");
            free(newBatch);
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n'); 

        newBatch->totalSales = 0;
        newBatch->next = newMed->batches;
        newMed->batches = newBatch;
        newMed->stockQuantity += newBatch->stock;

        insert_BatchNode(&Trees->Batch_root, newBatch);
    }

    int suppNo;
    printf("\nNumber of suppliers to add: ");
    if (scanf("%d", &suppNo) != 1 || suppNo < 0) {
        printf("Invalid number of suppliers.\n");
        freeBatchList(newMed->batches);
        free(newMed);
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    for (int i = 0; i < suppNo; i++) {
        int suppID;
        printf("\nSupplier %d:\n\tSupplier ID: ", i + 1);
        if (scanf("%d", &suppID) != 1) {
            printf("Invalid Supplier ID.\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        SuppNode* existingSupp = search_SuppNode_ByID(Trees->Supp_root, suppID);
        if (existingSupp) {
            for (int j = 0; j < MAX_MED_IN_SUPP; j++) {
                if (existingSupp->MedPtr[j] == NULL) {
                    existingSupp->MedPtr[j] = newMed;
                    printf("\tQuantity Supplied for this medication: ");
                    if (scanf("%d", &existingSupp->quantities[j]) != 1) {
                        printf("Invalid quantity.\n");
                        while (getchar() != '\n');
                        continue;
                    }
                    existingSupp->totalCostSupplied += (existingSupp->quantities[j]) * (newMed->pricePerUnit);
                    existingSupp->uniqueMedication++;
                    for (int k = 0; k < MAX_MED_IN_SUPP; k++){
                        if (newMed->suppliers[k] == NULL){
                            newMed->suppliers[k] = existingSupp;
                            break;
                        }
                    }
                    break;
                }
                if (j == MAX_MED_IN_SUPP - 1) {
                    printf("Supplier cannot handle more medications!\n");
                }
            }
        } else {
            SuppNode* newSupplier = (SuppNode*)malloc(sizeof(SuppNode));
            if (!newSupplier) {
                printf("Failed to allocate supplier.\n");
                freeSuppList(newMed->suppliers);
                freeBatchList(newMed->batches);
                free(newMed);
                return;
            }
            memset(newSupplier->MedPtr, NULL, sizeof(newSupplier->MedPtr));
            newSupplier->supplierID = suppID;
            printf("\tSupplier Name: ");
            fgets(newSupplier->supplierName, NAME_LEN, stdin);
            newSupplier->supplierName[strcspn(newSupplier->supplierName, "\n")] = '\0';
            printf("\tContact: ");
            fgets(newSupplier->contactInfo, NUM_LEN, stdin);
            newSupplier->contactInfo[strcspn(newSupplier->contactInfo, "\n")] = '\0';
            printf("\tQuantity Supplied for this medication: ");
            if (scanf("%d", &newSupplier->quantities[0]) != 1) {
                printf("Invalid quantity.\n");
                free(newSupplier);
                while (getchar() != '\n');
                continue;
            }
            newSupplier->MedPtr[0] = newMed;
            newSupplier->uniqueMedication = 1;
            newSupplier->totalCostSupplied = newSupplier->quantities[0] * newMed->pricePerUnit;
            for (int k = 0; k < MAX_MED_IN_SUPP; k++){
                if (newMed->suppliers[k] == NULL){
                    newMed->suppliers[k] = newSupplier;
                    break;
                }
            }
            insert_SuppNode(&Trees->Supp_root, newSupplier);
        }
    }
    insert_medNode(&Trees->Med_root, newMed);
    printf("Medication added successfully.\n");
}


void updateMedication(BTree* Trees){
    int medID;
    BT_MedNode* medRoot = Trees->Med_root;
    printf("\nInput the Medicine ID of medication you want to update: ");
    scanf("%d", &medID);
    
    MedNode* mNode = search_MedNode_ByID(medRoot, medID);   
    if (!mNode) {
        printf("Medication ID not found!\n");
        return;
    }
    
    int choice;
    printf("\nModify: \n1. Price Per Unit\n2. Stock\n3. Supplier\nChoice: ");
    scanf("%d", &choice);
    switch (choice){
        case 1:{
            printf("\nEnter the new price [previous price = rs %.2f]: ", mNode->pricePerUnit);
            scanf("%f", &mNode->pricePerUnit);
            while (getchar() != '\n');
            break;
        }
        case 2:{
            BatchNode* newBatch = (BatchNode*) malloc(sizeof(BatchNode));
            if (!newBatch) {
                printf("Memory allocation failed!\n");
                return;
            }
            newBatch->next = NULL;
            newBatch->medPtr = mNode; 
            printf("Enter the new Batch details:\n");

            while (getchar() != '\n' && getchar() != EOF);
            printf("\nBatch number: ");
            fgets(newBatch->batchNo, sizeof(newBatch->batchNo), stdin);
            newBatch->batchNo[strcspn(newBatch->batchNo, "\n")] = 0;
        
            char date[DATE_LEN];
            printf("\tExpiration date (DD-MM-YYYY): ");
            while ((getchar()) != '\n' && getchar() != EOF);
            fgets(date, sizeof(date), stdin);
            newBatch->expiryDate = check_and_correct_DATE(date);
        
            printf("\tNew stock quantity: ");
            scanf("%d", &newBatch->stock);
            if (newBatch->stock < 1) { 
                printf("Invalid quantity input!\n");
                free(newBatch);
                break;
            }
            
            newBatch->totalSales = 0;
            newBatch->next = mNode->batches;
            mNode->batches = newBatch;
            mNode->stockQuantity += newBatch->stock;

            insert_BatchNode((&Trees->Batch_root), newBatch);
            printf("Batch %s is successfully added!", newBatch->batchNo);
            break;
        }
        case 3:{
            SuppNode* sNode;
            int sID;
            printf("Which Supplier info do you want to update?\nSupplier ID:\n");
            scanf("%d", &sID);
            sNode = search_SuppNode_ByID(Trees->Supp_root, sID);

            if (!sNode){
                printf("No such supplier is found in %d!", medID);
                return;
            }
            
            //Update supplier Node
            int choice;
            printf("\n\nWhat do you want to update?\n1. Supplier Name\n2. Contact\n3. Quantity Supplied\nChoice: ");
            scanf("%d", &choice);
            switch(choice){
                case 1:{
                    printf("\n\tSupplied Name [Earlier: %s]: ",sNode->supplierName);
                    while (getchar() != '\n');
                    fgets(sNode->supplierName, sizeof(sNode->supplierName), stdin);
                    break;
                }
                case 2:{
                    printf("\nContact [Earlier: %s]: ", sNode->contactInfo);
                    fgets(sNode->contactInfo, sizeof(sNode->contactInfo), stdin);
                    break;
                }
                case 3:{
                    printf("\nNew quantity Supplied (New quan =  old + new stock): ");
                    int quan;
                    scanf("%d", &quan);
                    int i;
                    for (i = 0; i < MAX_MED_IN_SUPP && sNode->quantities[i] != NULL; i++){}
                    sNode->quantities[i] += quan;
                    break;
                }
            }
            break;
        }
        default:
            printf("\nInvalid Input!"); break;
    }
    printf("Medication is updated Successfully!");
}

void searchMedication(BTree* Trees){
    printf("\nSearch by:\n1. Medication ID\n2. Medication Name\n3. Supplier\nChoice: ");
    int choice;
    scanf("%d", &choice);
    getchar(); 

    switch(choice){
        case 1:{
            printf("\nEnter the medication ID: ");
            int medID;
            scanf("%d", &medID);

            MedNode* mNode = search_MedNode_ByID (Trees->Med_root, medID);

            if (mNode){
                printf("\nMedication found!\n");
                printf("\nMedication ID: %d", mNode->medID);
                printf("\nMedication Name: %s", mNode->medName);
                printf("\nPrice per unit: rs %.2f",mNode->pricePerUnit);
                printf("\nQuantity in stock: %d", mNode->stockQuantity);
                printf("\nTotal Sales: %d", mNode->totalSales);
                
                printf("\n\nBatch Details: ");
                BatchNode* btemp = mNode->batches;
                while(btemp){
                    printf("\n\tBatch Number: %s", btemp->batchNo);
                    printf("\n\tExpiration Date: %d", btemp->expiryDate);
                    printf("\n\tQuantity in stock: %d", btemp->stock);
                    printf("\n\tTotal Sales: %d", btemp->totalSales);
                    btemp = btemp->next;
                    printf("\n");
                }
                printf("\n\nSupplier Details: ");
                for (int i = 0; i <MAX_MED_IN_SUPP && mNode->suppliers[i] != NULL; i++){
                    printf("\n\tSupplier ID: %d", mNode->suppliers[i]->supplierID);
                    printf("\n\tSupplier Name: %s", mNode->suppliers[i]->supplierName);
                    printf("\n\tContact: %s", mNode->suppliers[i]->contactInfo);
                    int i = 0; i < mNode->
                    printf("\n\tQuantity supplied: %d", sTemp->quantitySupplied);
                    printf("\n");
                }
            }
            else
                printf("Medication NOT found!");

            break;
        }
        case 2:{
            printf("\nEnter Medication Name:  ");
            char medName[NAME_LEN];
            getchar();
            fgets(medName, sizeof(medName), stdin);
            medName[strcspn(medName, "\n")] = 0;
            int medID = searchIn_Med_Hash(medName);

            int found = 0;
            MedNode* mNode = search_MedNode_ByID (Trees->Med_root, medID);
            if (mNode) found = 1;

            if (found == 1){
                printf("\nMedication found!\n");
                printf("\nMedication ID: %d", mNode->medID);
                printf("\nMedication Name: %s", mNode->medName);
                printf("\nPrice per unit: rs %.2f",mNode->pricePerUnit);
                printf("\nQuantity in stock: %d", mNode->stockQuantity);
                printf("\nTotal Sales: %d", mNode->totalSales);
                
                printf("\n\nBatch Details: ");
                BatchNode* btemp = mNode->batches;
                while(btemp){
                    printf("\n\tBatch Number: %s", btemp->batchNo);
                    printf("\n\tExpiration Date: %d", btemp->expiryDate);
                    printf("\n\tQuantity in stock: %d", btemp->stock);
                    printf("\n\tTotal Sales: %d", btemp->totalSales);
                    btemp = btemp->next;
                    printf("\n");
                }
                printf("\n\nSupplier Details: ");
                SuppNode* sTemp = mNode->suppliers;
                while(sTemp){
                    printf("\n\tSupplier ID: %d", sTemp->supplierID);
                    printf("\n\tSupplier Name: %s", sTemp->supplierName);
                    printf("\n\tContact: %s", sTemp->contactInfo);
                    printf("\n\tQuantity supplied: %d", sTemp->quantitySupplied);
                    sTemp = sTemp->next;
                    printf("\n");
                }
            }
            else printf("Medication NOT found!");

            break;
        }
        case 3:{
            printf("Enter Supplier ID:");
            int sID;
            scanf("%d", &sID);
            printf("step 1\n");
            SuppNode* sNode = search_SuppNode_ByID(Trees->Supp_root, sID);
            printf("step 2\n");
            printf("%s\n", sNode->supplierName);
            printf("%d\n", sNode->uniqueMedication);
            printf("Medication ID: %d \n", sNode->MedPtr[0]->medID);
            for (int i = 0; i < sNode->uniqueMedication; i++) {
                printf("step 3");
                printf("Medication detail %d:\n", i+1);
                printf("Medication ID: %d \n", sNode->MedPtr[i]->medID);
                printf("Medication Name: %s\n", sNode->MedPtr[i]->medName);
                printf("Medication Price per Unit: %.2f\n", sNode->MedPtr[i]->pricePerUnit);
                printf("Medication stock quantity: %d\n", sNode->MedPtr[i]->stockQuantity);
                printf("\n");
            }
            break;
        }
        default: printf("Invalid Input!"); break;
    }
}

void deleteMedication(BTree* Trees) {
    int choice;
    printf("\nDelete by:\n1. Medication ID\n2. Batch Number\nChoice: ");
    if (scanf("%d", &choice) != 1) {
        printf("Invalid choice!\n");
        while (getchar() != '\n');
        return;
    }
    getchar();

    if (choice == 1) {
        int medID;
        printf("\nEnter Medication ID to delete: ");
        if (scanf("%d", &medID) != 1) {
            printf("Invalid ID!\n");
            while (getchar() != '\n');
            return;
        }
        getchar();
        MedNode* med = search_MedNode_ByID(Trees->Med_root, medID);
        if (!med) {
            printf("Medication not found!\n");
            return;
        }
        BatchNode* batch = med->batches;
        while (batch) {
            BatchNode* temp = batch;
            batch = batch->next;
            free(temp);
        }
        SuppNode* supp = med->suppliers;
        while (supp) {
            for (int i = 0; i < MAX_MED_IN_SUPP; i++) {
                if (supp->MedPtr[i] == med) {
                    supp->totalCostSupplied -= supp->quantities[i] * med->pricePerUnit;
                    supp->MedPtr[i] = NULL;
                    supp->quantities[i] = 0;
                    supp->uniqueMedication--;
                    break;
                }
            }
            supp = supp->next;
        }
        free(med);
        printf("Medication deleted successfully.\n");
    } else if (choice == 2) {
        int medID;
        char batchNo[NUM_LEN];
        printf("\nEnter Medication ID: ");
        if (scanf("%d", &medID) != 1) {
            printf("Invalid ID!\n");
            while (getchar() != '\n');
            return;
        }
        getchar();
        printf("Enter Batch Number: ");
        fgets(batchNo, NUM_LEN, stdin);
        batchNo[strcspn(batchNo, "\n")] = '\0';
        MedNode* med = search_MedNode_ByID(Trees->Med_root, medID);
        if (!med) {
            printf("Medication not found!\n");
            return;
        }
        BatchNode* prev = NULL;
        BatchNode* batch = med->batches;
        while (batch && strcmp(batch->batchNo, batchNo) != 0) {
            prev = batch;
            batch = batch->next;
        }
        if (!batch) {
            printf("Batch not found!\n");
            return;
        }
        if (prev) {
            prev->next = batch->next;
        } else {
            med->batches = batch->next;
        }
        med->stockQuantity -= batch->stock;
        free(batch);
        printf("Batch deleted successfully.\n");
    } else {
        printf("Invalid choice!\n");
    }
}

void stockAlerts(BTree* Trees) {
    int found = 0;
    printf("\nStock Alert for (format: ID, Name, stock): ");

    TraverseFor_StockAlerts(Trees->Med_root, &found);
    
    if(found == 0)
        printf("\nAll medications have proper stock.");
}

void checkExpiration(BTree* Trees){
    int found = 0;
    printf("\nEnter today's date: ");
    char today[DATE_LEN];
    scanf("%s", today);
    int todayDate = check_and_correct_DATE(today);
    
    printf("\nFollowing medication are either expired or approaching expiry date [format: Medication ID, Batch No., Quantity, Expiry Date]: ");

    TraverseFor_checkExpiration(Trees->Batch_root, todayDate, &found);

    if(found == 0) printf("No medications are expired!");
}

void check_MedicationBet_DATES(BTree* Trees){
    char dt1[DATE_LEN], dt2[DATE_LEN];
    printf("Enter Date 1: ");
    while (getchar() != '\n' && getchar() != EOF); // Flush input buffer
    fgets (dt1, DATE_LEN, stdin);
    printf("Enter Date 2: ");
    while (getchar() != '\n' && getchar() != EOF);
    fgets (dt2, DATE_LEN, stdin);
    getchar();
    
    int date1 = check_and_correct_DATE (dt1);
    int date2 = check_and_correct_DATE (dt2);

    int found = 0;
    TraverseFor_check_MedicationBet_DATES(Trees->Batch_root, date1, date2, &found);

    if (!found) printf("No medications found between the specified dates.");
}

void recordSales(BTree* Trees){
    printf("\nEnter these 3 details of sold Medication: ");
    printf("\n1. Medication ID: ");
    int medID;
    scanf("%d", &medID);
    getchar();

    printf("\n2. Batch Number of Medication Id %d: ", medID);
    char batchNo[NUM_LEN];
    fgets(batchNo, sizeof(batchNo), stdin);
    batchNo[strcspn(batchNo, "\n")] = 0;

    printf("\n3. Number of units sold: ");
    int sold;
    scanf("%d", &sold);
    
    MedNode* mNode = search_MedNode_ByID(Trees->Med_root, medID);
    if (!mNode) {
        printf("Medication not found!\n");
        return;
    }
    BatchNode* bNode = mNode->batches, *prevBatch = NULL;

    while (bNode != NULL){
        if (strcmp(bNode->batchNo, batchNo) == 0){
            if (bNode->stock > sold){
                bNode->stock -= sold;
                bNode->totalSales += sold;
                mNode->stockQuantity -= sold;
                mNode->totalSales += sold;
            }
            else if (bNode->stock == sold) {
                if (prevBatch) prevBatch->next = bNode->next;
                else mNode->batches = bNode->next;
                
                // deleteBatchFromBTree(Trees->Batch_root, bNode);
                free(bNode);
            }
            else    
                printf("Stock is Insufficient!");
            return;
        }
        bNode = bNode->next;
    }

    printf("Sales are recorded!");
}

void supplierManagement(BTree* Trees) {
    BT_SuppNode* suppRoot = Trees->Supp_root;
    int choice, medID;
    printf("Enter medications Id of supplied medicine: ");
    scanf("%d", &medID);
    MedNode* med = search_MedNode_ByID(Trees->Med_root, medID);
    if (med == NULL) {
        printf("Invalid Medication ID! No such medication found.\n");
        return;
    }
    do {
        printf("\n==== Supplier Management ====");
        printf("\n1. Add Supplier\n2. Update Supplier\n3. Delete Supplier\n4. Search Supplier");
        printf("\n0. Back to Main Menu\n\nSelect an option: ");
        scanf("%d", &choice);
        
        switch(choice) {
            case 1: addSupplierToMedication(&suppRoot, med); break;
            case 2: updateSupplierInMedication(suppRoot, med); break;
            case 3: deleteSupplierFromMedication(&suppRoot, med); break;
            case 4: searchSupplierInMedication(suppRoot); break;
            case 0: printf("\nReturning to main menu..."); break;
            default: 
                printf("\nInvalid choice! Please enter a valid option.\n");
                continue;
        }
    } while(choice != 0);
}

void findAll_rounderSupplier(BTree* Trees) {
    SuppNode* suppliers[1000]; // Adjust size as needed
    int count = 0;
    collectSuppliers(Trees->Supp_root, suppliers, &count);
    if (count == 0) {
        printf("No suppliers found!\n");
        return;
    }
    qsort(suppliers, count, sizeof(SuppNode*), compareUniqueMed);
    printf("\nTop All-Rounder Supplier:\n");
    printf("ID: %d, Name: %s, Unique Medications: %d\n", 
           suppliers[0]->supplierID, suppliers[0]->supplierName, suppliers[0]->uniqueMedication);
    printf("\nTop 10 All-Rounder Suppliers:\n");
    for (int i = 0; i < 10 && i < count; i++) {
        printf("%d. ID: %d, Name: %s, Unique Medications: %d\n", 
               i + 1, suppliers[i]->supplierID, suppliers[i]->supplierName, suppliers[i]->uniqueMedication);
    }
}

void findTop10LargestTurnover(BTree* Trees) {
    SuppNode* suppliers[1000]; // Adjust size as needed
    int count = 0;
    collectSuppliers(Trees->Supp_root, suppliers, &count);
    if (count == 0) {
        printf("No suppliers found!\n");
        return;
    }
    qsort(suppliers, count, sizeof(SuppNode*), compareTurnover);
    printf("\nSupplier with Largest Turnover:\n");
    printf("ID: %d, Name: %s, Total Cost Supplied: %.2f\n", 
           suppliers[0]->supplierID, suppliers[0]->supplierName, suppliers[0]->totalCostSupplied);
    printf("\nTop 10 Suppliers by Turnover:\n");
    for (int i = 0; i < 10 && i < count; i++) {
        printf("%d. ID: %d, Name: %s, Total Cost Supplied: %.2f\n", 
               i + 1, suppliers[i]->supplierID, suppliers[i]->supplierName, suppliers[i]->totalCostSupplied);
    }
}