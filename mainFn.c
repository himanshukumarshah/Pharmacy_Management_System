#include "prototypes.h"

void addNewMedication(BT_roots* Trees) {
    MedNode* newMed = createMedNode();
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

    if (searchIn_Med_Hash(newMed->medName)) {
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
    printf("Reorder Level: ");
    if (scanf("%d", &newMed->reorderLevel) != 1) {
        printf("\nInvalid price.");
        free(newMed);
        while (getchar() != '\n');
        return;
    }
    
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

    for (int i = 1; i <= bNo; i++) {
        BatchNode* newBatch = createBatchNode();
        if (!newBatch) {
            printf("Failed to allocate batch.\n");
            freeBatchList(newMed->batches); 
            free(newMed);
            return;
        }
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

        BT_insert(Trees->BatchTree, newBatch);
    }

    //Adding Suppliers

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

        SuppNode ID;
        ID.supplierID = suppID;
        SuppNode* existingSupp = search_ByID(Trees->SupplierTree, &ID);

        if (existingSupp) {
            printf("This Supplier is already exist. Adding in the existing supplier...\n");
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
            printf("New Supplier!\n");
            SuppNode* newSupplier = createSuppNode();
            if (!newSupplier) {
                printf("Failed to allocate supplier.\n");
                for(int i = 0; newMed->suppliers[i] != NULL; i++)
                    free(newMed->suppliers[i]);
                freeBatchList(newMed->batches);
                free(newMed);
                return;
            }
            for(int i = 0; i < MAX_MED_IN_SUPP; i++)
                newSupplier->MedPtr[i] = NULL;
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
            getchar();

            newSupplier->MedPtr[0] = newMed;
            newSupplier->uniqueMedication = 1;
            newSupplier->totalCostSupplied = newSupplier->quantities[0] * newMed->pricePerUnit;
            for (int k = 0; k < MAX_MED_IN_SUPP; k++){
                if (newMed->suppliers[k] == NULL){
                    newMed->suppliers[k] = newSupplier;
                    break;
                }
                if(k == MAX_MED_IN_SUPP - 1){
                    printf("Maximim Supplier in the medicine reached.");
                }
            }
            BT_insert(Trees->SupplierTree, newSupplier);
        }
    }
    BT_insert(Trees->medTree, newMed);
    printf("Medication added successfully.\n");
}

void updateMedication(BT_roots* Trees){
    int medID;
    printf("\nInput the Medicine ID of medication you want to update: ");
    scanf("%d", &medID);

    MedNode ID;
    ID.medID = medID;
    MedNode* mNode = search_ByID(Trees->medTree, &ID);   

    if (!mNode) {
        printf("Medication ID not found!\n");
        return;
    }
    
    int choice;
    printf("\nWant to Modify? :\n1. Price Per Unit\n2. Stock\n3. Supplier\n\nChoice: ");
    scanf("%d", &choice);
    switch (choice){
        case 1:{
            printf("\nEnter the new price [previous price = rs %.2f]: ", mNode->pricePerUnit);
            scanf("%f", &mNode->pricePerUnit);
            while (getchar() != '\n');
            break;
        }
        case 2:{
            printf("\n1. Adding a new Batch\n2. Modify the precious stock\n Choice: ");
            int ch;
            scanf("%d", &ch);
            switch(ch){
                case 1:{
                    BatchNode* newBatch = createBatchNode();
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
                    
                    // while(getchar() != '\n');
                    char date[DATE_LEN];
                    printf("\tExpiration date (DD-MM-YYYY): ");
                    fgets(date, sizeof(date), stdin);
                    date[strcspn(date, "\n")] = '\0';
                    printf("DEBUG: Date entered = '%s' (length = %zu)\n", date, strlen(date));

                    newBatch->expiryDate = check_and_correct_DATE(date);
                    if(newBatch->expiryDate == -1)
                        return;
                
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

                    BT_insert(Trees->BatchTree, newBatch);
                    printf("\nBatch %s is successfully added!", newBatch->batchNo);
                    break;
                }
                case 2: {
                    printf("\nThese Batches (| Batch no. | stock |) are there for medication ID %d:", medID);
                    BatchNode* batch = mNode->batches;
                    while(batch != NULL){
                        printf("\n| %s | %d |", batch->batchNo, batch->stock);
                        batch = batch->next;
                    }

                    printf("\n\nEnter batch number you want to update: ");
                    char bch[NUM_LEN];
                    getchar();
                    fgets(bch, NUM_LEN, stdin);
                    bch[strcspn(bch, "\n")] = '\0';

                    batch = mNode->batches;
                    while(batch != NULL){
                        if(strcmp(batch->batchNo, bch) == 0) break;
                        batch = batch->next;
                    }
                    if(!batch){
                        printf("\nInvalid Batch Nnumber.");
                        return;
                    }
                    int st;
                    printf("\nEnter the new stock: ");
                    scanf("%d", &st);
                    mNode->stockQuantity += st - batch->stock;
                    batch->stock = st;
                    printf("\nBatch (%s)'s stock is updated successfully to %d.", batch->batchNo, batch->stock);

                    break;
                }
                default:
                    printf("\nInvalid Choice!");
                    break;
            }
            break;
        }
        case 3:{
            SuppNode* sNode = NULL;
            int sID;

            printf("\nFollowing are the suppliers ([ID] Name) of medication [%d] %s: ", mNode->medID, mNode->medName);
            for(int i = 0; i < MAX_MED_IN_SUPP && mNode->suppliers[i] != NULL; i++){
                printf("\n[%d] %s", mNode->suppliers[i]->supplierID, mNode->suppliers[i]->supplierName);
            }

            printf("\n\nWhich Supplier info do you want to update?\nSupplier ID: ");
            scanf("%d", &sID);

            for(int i = 0; i < MAX_MED_IN_SUPP && mNode->suppliers[i] != NULL; i++){
                if(mNode->suppliers[i]->supplierID == sID){
                    sNode = mNode->suppliers[i];
                    break;
                }
            }

            if (!sNode){
                printf("\nNo such supplier is found in %d!", medID);
                return;
            }
            
            //Update supplier Node
            int ch;
            do{
                printf("\n\nWhat do you want to update?\n1. Supplier Name\n2. Contact\n3. Quantity Supplied\n0. Back\nChoice: ");
                scanf("%d", &ch);
                switch(ch){
                    case 1:{
                        printf("\n\tSupplied Name [Earlier: %s]: ",sNode->supplierName);
                        while (getchar() != '\n');
                        fgets(sNode->supplierName, sizeof(sNode->supplierName), stdin);
                        sNode->supplierName[strcspn(sNode->supplierName, "\n")] = '\0';
                        break;
                    }
                    case 2:{
                        while (getchar() != '\n');
                        printf("\nContact [Earlier: %s]: ", sNode->contactInfo);
                        fgets(sNode->contactInfo, sizeof(sNode->contactInfo), stdin);
                        sNode->contactInfo[strcspn(sNode->contactInfo, "\n")] = '\0';
                        break;
                    }
                    case 3:{
                        printf("\nNew quantity Supplied (New Stock =  old + new quantity supplied): ");
                        int quan;
                        scanf("%d", &quan);
                        int i;
                        for(i = 0; i < MAX_MED_IN_SUPP && sNode->MedPtr[i]->medID != mNode->medID; i++){}

                        sNode->quantities[i] += quan;
                        sNode->totalCostSupplied += quan * mNode->pricePerUnit;

                        break;
                    }
                    case 0: break;
                    default:
                        printf("\nInavalid choice!"); break;
                }
            } while(ch != 0);
            break;
        }
        default:
            printf("\nInvalid Input!"); break;
    }
    printf("\nMedication is updated Successfully!");
}

void searchMedication(BT_roots* Trees){
    printf("\nSearch by:\n1. Medication ID\n2. Medication Name\n3. Supplier\nChoice: ");
    int choice;
    scanf("%d", &choice);
    getchar(); 

    switch(choice){
        case 1:{
            printf("\nEnter the medication ID: ");
            int medID;
            scanf("%d", &medID);

            MedNode ID;
            ID.medID = medID;
            MedNode* mNode = search_ByID(Trees->medTree, &ID);

            print_Medication(mNode);
            break;
        }
        case 2:{
            printf("\nEnter Medication Name:  ");
            char medName[NAME_LEN];
            fgets(medName, sizeof(medName), stdin);
            medName[strcspn(medName, "\n")] = '\0';
            int medID = searchIn_Med_Hash(medName);
            if (medID == 0) {
                printf("Medication not found!\n");
                return;
            }

            MedNode ID;
            ID.medID = medID;
            MedNode* mNode = search_ByID(Trees->medTree, &ID);

            print_Medication(mNode);
            break;
        }
        case 3:{
            printf("Enter Supplier ID:");
            int sID;
            scanf("%d", &sID);

            SuppNode ID;
            ID.supplierID = sID;
            SuppNode* sNode = search_ByID(Trees->SupplierTree, &ID);

            if(sNode){
                printf("\n[ID] Supplier Name: [%d] %s", sID, sNode->supplierName);
                printf("\nContact: %s", sNode->contactInfo);
                printf("\nUnique medication Supplied: %d", sNode->uniqueMedication);
                printf("\nTotal Cost Supplied: rs %.2f", sNode->totalCostSupplied);
                printf("\n\nMedications Supplied:");
                printf("\n| S.No. | ID | Name | Price/Unit (rs) | Quantity Supplied(pcs) |");
                for (int i = 0; i < MAX_MED_IN_SUPP && sNode->MedPtr[i] != NULL; i++) {
                    MedNode* med = sNode->MedPtr[i];
                    printf("\n| %d | %d | %s | %.2f | %d |", i + 1, med->medID, med->medName, med->pricePerUnit, sNode->quantities[i]);
                }
            }
            else printf("Supplier Not found!");
            break;
        }
        default: printf("Invalid Input!"); break;
    }
}


void deleteMedication(BT_roots* Trees) {
    int choice;
    printf("\nDelete by:\n1. Medication ID\n2. Batch Number\nChoice: ");
    while (scanf("%d", &choice) != 1) {
        if(choice == 0) return;
        while (getchar() != '\n');
        printf("\nInvalid choice!");
        printf("\nDelete by:\n1. Medication ID\n2. Batch Number\n0. Exit\nChoice: ");
    }
    getchar();

    if (choice == 1) {
        int medID;
        printf("\nEnter Medication ID to delete: ");
        while (scanf("%d", &medID) != 1) {
            if(medID == 0) return;
            while (getchar() != '\n');
            printf("\nInvalid ID!");
            printf("\nEnter Medication ID to delete (or 0 to exit): ");
        }
        getchar();

        MedNode ID;
        ID.medID = medID;
        MedNode* med = search_ByID(Trees->medTree, &ID);

        if (!med) {
            printf("Medication not found!\n");
            return;
        }
        printf("\nMedication: [%d] %s", med->medID, med->medName);
        printf("\nBatches ([Batch no.], Stock): ");
        BatchNode* batch = med->batches;
        while(batch){
            printf("\n[%s], %d", batch->batchNo, batch->stock);
            batch = batch->next;
        }
        printf("\n\nDo you want to delete all the batches and details of medications? (y/n) ");
        char ch;
        scanf("%c", &ch);
        if(tolower(ch) == 'n') return;

        batch = med->batches;
        while (batch) {
            BT_Delete(Trees->BatchTree, batch);
            batch = batch->next;
        }

        SuppNode* supp;

        for(int i = 0; i < MAX_MED_IN_SUPP && med->suppliers[i] != NULL; i++){
            supp = med->suppliers[i];
            for(int j = 0; j < MAX_MED_IN_SUPP; j++){
                if(med->medID == supp->MedPtr[j]->medID){
                    supp->totalCostSupplied -= med->pricePerUnit * supp->quantities[j];
                    for(int k = j + 1; k < MAX_MED_IN_SUPP; k++){
                        supp->MedPtr[k - 1] = supp->MedPtr[k];
                        supp->quantities[k - 1] = supp->quantities[k];
                    }
                    supp->MedPtr[MAX_MED_IN_SUPP - 1] = NULL;
                    supp->quantities[MAX_MED_IN_SUPP - 1] = 0;
                    supp->uniqueMedication--;
                    break;
                }
            }
        }
        BT_Delete(Trees->medTree, med);
        printf("\nMedication deleted successfully.\n");
    } 
    else if (choice == 2) {
        char batchNo[NUM_LEN];
        printf("Enter Batch Number: ");
        fgets(batchNo, NUM_LEN, stdin);
        batchNo[strcspn(batchNo, "\n")] = '\0';
        BatchNode* batch = search_BatchNode_ByBatchNo(Trees->BatchTree->root, batchNo);
        if (!batch) {
            printf("Batch not found!\n");
            return;
        }

        MedNode* med = batch->medPtr;
        BatchNode* prev = NULL;
        BatchNode* curr = med->batches;

        while (curr && strcmp(curr->batchNo, batch->batchNo) != 0) {
            prev = curr;
            curr = curr->next;
        }
        
        if (prev) 
            prev->next = curr->next;
        else 
            med->batches = curr->next;

        med->stockQuantity -= batch->stock;

        BT_Delete(Trees->BatchTree, batch);

        printf("Batch deleted successfully.\n");
    } 
    else printf("Invalid choice!\n");
}

void stockAlerts(BT_roots* Trees){
    int found = 0;
    printf("\n!!Stock Alert!!");
    printf("\n| [ID] Name | Price/Unit (rs) | Quantity in stock |");
    traverse_StockAlerts(Trees->medTree->root, &found);
    if(!found) printf("\nAll medication are in proper stock.");
}

void checkExpirationDates(BT_roots* Trees){
    char today[DATE_LEN];
    printf("\nEnter today's date: ");
    fgets(today, DATE_LEN, stdin);
    today[strcspn(today, "\n")] = '\0';
    int todayDate = check_and_correct_DATE(today);

    int found = 0;
    printf("\n!!Expiration dates approaching!!");
    printf("\n\n| [ID] Medicine Name\t| Batch no.\t| Stock\t| Expiry Date |\n");
    traverse_ExpirationDates(Trees->BatchTree->root, todayDate, &found);
    if(!found) printf("\nNo Medications' expiry date is within 30 days,");
}

void MedBetweenDates(BT_roots* Trees){
    char dt1[DATE_LEN], dt2[DATE_LEN];
    printf("\nEnter the 'from' date: ");
    fgets(dt1, DATE_LEN, stdin);
    dt1[strcspn(dt1, "\n")] = '\0';
    int date1 = check_and_correct_DATE(dt1);
    printf("\nEnter the 'to' date: ");
    fgets(dt2, DATE_LEN, stdin);
    dt2[strcspn(dt2, "\n")] = '\0';
    int date2 = check_and_correct_DATE(dt2);
    
    int found = 0;
    printf("\nMedications whose expiry dates are from %s to %s: ", dt1, dt2);
    printf("\n\n| [ID] Medicine Name\t| Batch no.\t| Stock\t| Expiry Date |\n");
    rangeSearch_Dates(Trees->BatchTree->root, date1, date2, &found);
    if(!found) printf("\nNo medication found within this range.");
}

void recordSales(BT_roots* Trees){
    printf("\nEnter these details of sold Medication: ");
    printf("\n1. Medication ID: ");
    int medID;
    scanf("%d", &medID);
    getchar();

    MedNode ID;
    ID.medID = medID;
    MedNode* mNode = search_ByID(Trees->medTree, &ID);
    if (!mNode) {
        printf("Medication not found!\n");
        return;
    }

    printf("\nFollowing are the batches (batch no., stock) of medication [%d] %s: ", mNode->medID, mNode->medName);
    BatchNode* bt = mNode->batches;
    while(bt){
        printf("\n%s, %d", bt->batchNo, bt->stock);
        bt = bt->next;
    }

    printf("\n\n2. Batch Number of Medication Id %d: ", medID);
    char batchNo[NUM_LEN];
    fgets(batchNo, sizeof(batchNo), stdin);
    batchNo[strcspn(batchNo, "\n")] = '\0';

    printf("\n3. Number of units sold: ");
    int sold;
    scanf("%d", &sold);
    getchar();

    BatchNode* bNode = mNode->batches, *prevNode = NULL;
    if(!bNode){
        printf("\nNo batch found!");
        return;
    }

    printf("\nDo you want to continue recording %d unit sales of [%d] %s (Batch: %s)? (y/n) ", sold, mNode->medID, mNode->medName, bNode->batchNo);
    char ans;
    scanf("%c", &ans);
    if(tolower(ans) == 'n'){
        printf("\nGoing back...");
        return;
    } 
        
    while (bNode){
        if (strcmp(bNode->batchNo, batchNo) == 0){
            if (bNode->stock > sold){
                bNode->stock -= sold;
                bNode->totalSales += sold;
                mNode->stockQuantity -= sold;
                mNode->totalSales += sold;

                printf("Sales are recorded!");
            }
            else if (bNode->stock == sold) {
                if (prevNode) prevNode->next = bNode->next;
                else mNode->batches = bNode->next;
                
                BT_Delete(Trees->BatchTree, bNode);
                printf("Sales are recorded!");
            }
            else printf("\nStock is Insufficient!");

            break;
        }
        prevNode = bNode;
        bNode = bNode->next;
    }
}

void supplierManagement(BT_roots* Trees) {
    int choice;
    do {
        printf("\n\n==== Supplier Management ====");
        printf("\n1. Add Supplier\n2. Update Supplier\n3. Delete Supplier\n4. Search Supplier\n0. Back\n\nSelect an option: ");
        scanf("%d", &choice);
        
        switch(choice) {
            case 1: addSupplier(Trees); break;
            case 2: updateSupplier(Trees->SupplierTree); break;
            case 3: deleteSupplier(Trees->SupplierTree); break;
            case 4: searchSupplier(Trees->SupplierTree); break;
            case 0: printf("\nReturning back..."); break;
            default: 
                printf("\nInvalid choice! Please enter a valid option.\n");
                continue;
        }
    } while(choice != 0);
}

void addSupplier(BT_roots* Trees){
    int medID;
    printf("\nEnter medications Id of supplied medicine: ");
    scanf("%d", &medID);
    getchar();

    MedNode ID;
    ID.medID = medID;
    MedNode* med = search_ByID(Trees->medTree, &ID);

    if (!med) {
        printf("\nNo such medication found.\nAdd a new medication.");
        return;
    }

    SuppNode* newSupp = createSuppNode();
    if (!newSupp) return;
    
    printf("\nEnter following details:");
    printf("\nSupplier ID: ");
    scanf("%d", &newSupp->supplierID);
    getchar();
    
    printf("\nSupplier Name: ");
    fgets(newSupp->supplierName, NAME_LEN, stdin);
    newSupp->supplierName[strcspn(newSupp->supplierName, "\n")] = '\0';
    
    printf("Enter Contact Info: ");
    fgets(newSupp->contactInfo, NUM_LEN, stdin);
    newSupp->contactInfo[strcspn(newSupp->contactInfo, "\n")] = '\0';
    
    printf("Enter Quantity Supplied: ");
    scanf("%d", &newSupp->quantities[0]);
    getchar();
    
    newSupp->uniqueMedication = 1;
    newSupp->totalCostSupplied = med->pricePerUnit * newSupp->quantities[0];
    newSupp->MedPtr[0] = med;
    
    med->stockQuantity += newSupp->quantities[0];
    
    for(int i = 0; i < MAX_MED_IN_SUPP; i++){
        if(med->suppliers[i] == NULL){
            med->suppliers[i] = newSupp;
            break;
        }
        if( i == MAX_MED_IN_SUPP - 1){
            printf("\nThe medication has maximum suppliers.");
            free(newSupp);
            return;
        }
    }

    BT_insert(Trees->SupplierTree, newSupp);
    printf("Supplier added successfully.\n");
}

void updateSupplier(BTree* suppTree){
    int suppID;
    printf("\nEnter Supplier ID: ");
    scanf("%d", &suppID);
    
    SuppNode sID;
    sID.supplierID = suppID;
    SuppNode* supp = search_ByID(suppTree, &sID);

    if (!supp) {
        printf("\nSupplier with ID %d not found.", suppID);
        return;
    }

    MedNode* med;

    int medID;
    printf("\nEnter medications Id of supplied medicine: ");
    scanf("%d", &medID);

    int i = 0;
    while(i < MAX_MED_IN_SUPP && supp->MedPtr[i]->medID != medID) i++;
    if(i == MAX_MED_IN_SUPP){
        printf("\nSupplier [%d] %s is not supplying medicne %d.", supp->supplierID, supp->supplierName, medID);
        return;
    }
    med = supp->MedPtr[i];

    printf("\nCurrent Supplier Details:");
    printf("\n1. Name: %s", supp->supplierName);
    printf("\n2. Contact: %s", supp->contactInfo);
    printf("\n3. Quantity Supplied: %d", supp->quantities[i]);
    
    int choice;
    do{
        printf("\n\nWhat do you want to update?");
        printf("\n1. Name\n2. Contact\n3. Quantity Supplied\n0. Back\nChoice: ");
        scanf("%d", &choice);
        getchar();
        
        switch(choice) {
            case 1:
                printf("\nEnter new name: ");
                fgets(supp->supplierName, NAME_LEN, stdin);
                supp->supplierName[strcspn(supp->supplierName, "\n")] = '\0';
                break;
            case 2:
                printf("\nEnter new contact: ");
                fgets(supp->contactInfo, NUM_LEN, stdin);
                supp->contactInfo[strcspn(supp->contactInfo, "\n")] = '\0';
                break;
            case 3: {
                int newQty;
                printf("\nEnter new quantity supplied of medication ID %d: ", medID);
                scanf("%d", &newQty);
                getchar();
                supp->totalCostSupplied += med->pricePerUnit * (newQty - supp->quantities[i]);
                supp->quantities[i] = newQty;
                break;
            }
            case 0: printf("Returning back..."); break;
            default:
                printf("\nInvalid choice.");
        }
    } while (choice != 0);

    printf("\nSupplier updated successfully.\n");
}

void deleteSupplier(BTree* suppTree){
    int suppID;
    printf("\nEnter supplier ID: ");
    scanf("%d", &suppID);
    getchar();

    SuppNode ID;
    ID.supplierID = suppID;
    SuppNode* supp = search_ByID(suppTree, &ID);

    if(!supp){
        printf("\nSupplier Not found.");
        return;
    }

    printf("\nFollowing are the details of the supplier %d: ", suppID);
    printf("\nName: %s\nContact: %s", supp->supplierName, supp->contactInfo);
    printf("\n\nMedication supplied:\n| Medication Name | Quantity |");
    for(int i = 0; i < MAX_MED_IN_SUPP && supp->MedPtr[i] != NULL; i++){
        printf("\n| [%d] %s | %d |", supp->MedPtr[i]->medID, supp->MedPtr[i]->medName, supp->quantities[i]);
    }
    printf("\n\nDo you want to delete all details of the supplier? (y/n) ");
    char ans;
    scanf("%c", &ans);
    if(tolower(ans) == 'n'){
        printf("\nReturning back...");
        return;
    }
    
    for(int i = 0; i < MAX_MED_IN_SUPP && supp->MedPtr[i] != NULL; i++){
        MedNode* med = supp->MedPtr[i];
        for(int j = 0; j < MAX_MED_IN_SUPP && med->suppliers[j] != NULL; j++){
            if(med->suppliers[j]->supplierID == supp->supplierID){
                int k;
                for(k = j; k < MAX_MED_IN_SUPP && med->suppliers[k] != NULL; k++){
                    med->suppliers[k] = med->suppliers[k + 1];
                }
                med->suppliers[k] = NULL;
            }
        }
    }

    BT_Delete(suppTree, supp);

    printf("\nSupplier %d has been deleted successfully.", suppID);
}

void searchSupplier(BTree* suppTree){
    int suppID;
    printf("\nEnter supplier ID: ");
    scanf("%d", &suppID);

    SuppNode ID;
    ID.supplierID = suppID;
    SuppNode* supp = search_ByID(suppTree, &ID);

    if(!supp){
        printf("\nSupplier Not found.");
        return;
    }

    printf("\nFollowing are the details of the supplier ID %d: ", suppID);
    printf("\nName: %s\nContact: %s", supp->supplierName, supp->contactInfo);
    printf("\nUnique medication: %d\nRevenue: %.2f", supp->uniqueMedication, supp->totalCostSupplied);
    printf("\n\nMedication supplied:\n| Medication | Quantity supplied |");
    for(int i = 0; i < MAX_MED_IN_SUPP && supp->MedPtr[i] != NULL; i++){
        printf("\n| [%d] %s | %d |", supp->MedPtr[i]->medID, supp->MedPtr[i]->medName, supp->quantities[i]);
    }
}

void FindAllRounderSupplier(BTree* suppTree){
    SuppNode* supp[501];
    int count = 0;
    collectSuppliers(suppTree->root, supp, &count);
    if(count == 0){
        printf("\nNo supplier found.");
        return;
    }
    qsort(supp, count, sizeof(SuppNode*), cmpUniqueMed);

    printf("\nTop All-Rounder Supplier:");
    printf("\nName: [%d] %s\nContact: %s\nUnique Medications: %d", supp[0]->supplierID, supp[0]->supplierName, supp[0]->contactInfo, supp[0]->uniqueMedication);

    printf("\n\nTop 10 All-Rounder Suppliers:\n");
    printf("\n| S.No. | [ID] Name | Contact | Unique Medication |");
    for (int i = 0; i < 10 && i < count; i++) {
        printf("\n| %d | [%d] %s ] | %s | %d |", i + 1, supp[i]->supplierID, supp[i]->supplierName, supp[i]->contactInfo, supp[i]->uniqueMedication);
    }
}
void FindLargestTuroverSupplier(BTree* suppTree){
    SuppNode* supp[501];
    int count = 0;
    collectSuppliers(suppTree->root, supp, &count);
    if(count == 0){
        printf("\nNo supplier found.");
        return;
    }
    qsort(supp, count, sizeof(SuppNode*), cmpTurnover);

    printf("\nLargest turnover supplier:");
    printf("\nName: [%d] %s\nContact: %s\nTurnover: %.2f", supp[0]->supplierID, supp[0]->supplierName, supp[0]->contactInfo, supp[0]->totalCostSupplied);

    printf("\n\nTop 10 largest turnover suppliers:\n");
    printf("\n| S.No. | [ID] Name | Contact | Turnover |");
    for (int i = 0; i < 10 && i < count; i++) {
        printf("\n| %d | [%d] %s ] | %s | %.2f |", i + 1, supp[i]->supplierID, supp[i]->supplierName, supp[i]->contactInfo, supp[i]->totalCostSupplied);
    }
}