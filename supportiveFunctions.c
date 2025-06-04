#include "structures.h"

/*--------------- Hash Table ---------------*/

unsigned int hashFn(const char *name){
    unsigned int hash = 5831;
    while (*name != '\0'){
        hash = hash * 31 + *name;
        name++;
    }
    return hash % TABLE_SIZE;
}

void insertIn_Med_Hash(char* medName, int medID){
    MedName* newNode = (MedName*)malloc(sizeof(MedName));
    if (!newNode) {
        printf("Memory allocation failed\n");
        return;
    }
    newNode->medID = medID;
    strcpy(newNode->medName, medName);
    newNode->next = NULL;

    unsigned int idx = hashFn(medName);

    newNode->next = MedTable[idx];
    MedTable[idx] = newNode;
}

int searchIn_Med_Hash(char* medName){
    unsigned int idx = hashFn(medName);
    MedName* curr = MedTable[idx];
    int medID = -1; 
    while (curr != NULL){
        if (strcmp(medName, curr->medName) == 0){
            medID = curr->medID;
            break;
        }
        curr = curr->next;
    }
    return medID; 
}

// void insertIn_Supp_Hash(char *suppName, int suppID){
//     SuppName* newNode = (SuppName*)malloc(sizeof(SuppName));
//     newNode->suppID = suppID;
//     strcpy(newNode->suppName, suppName);
//     newNode->next = NULL;

//     unsigned int idx = hashFn(suppName);

//     newNode->next = SuppTable[idx];
//     SuppTable[idx]->next = newNode;
// }

// int searchIn_Supp_Hash(char* suppName){
//     unsigned int idx = hashFn(suppName);

//     SuppName* curr = SuppTable[idx];
//     bool found = false;
//     int suppID;
//     while (!found && curr != NULL){
//         if (strcmp(suppName, curr->suppName)){
//             suppID = curr->suppID;
//             found = true;
//         }
//         curr = curr->next;
//     }
//     return suppID;
// }

/*--------------- Insertion of MedNode, BatchNode, and SuppNode ---------------*/

void insert_medNode(BT_MedNode** medRoot, MedNode* newMed){
    MedNode* upkey;
    if (!(*medRoot)){
        *medRoot = CREATE_NODE(BT_MedNode, true);
        (*medRoot)->keys[0] = newMed;
        (*medRoot)->noOfKeys = 1;
    }
    else {
        BT_MedNode* newRight = NULL;
        *medRoot = insertInternal_Med(*medRoot, newMed, &upkey, &newRight);
        if (newRight != NULL){
            BT_MedNode* newNode = CREATE_NODE(BT_MedNode, false);
            newNode->keys[0] = upkey;
            newNode->children[0] = *medRoot;
            newNode->children[1] = newRight;
            newNode->noOfKeys = 1;
            *medRoot = newNode;
        }
    }
}

BT_MedNode* insertInternal_Med(BT_MedNode* Node, MedNode* newMed, MedNode** upkey, BT_MedNode** newRight){
    BT_MedNode* ret_Node = Node;
    if (Node == NULL) return ret_Node;
    if (Node->isLeaf){
        if(Node->noOfKeys < ORDER - 1){
            int i = Node->noOfKeys - 1;
            while (i >= 0 && Node->keys[i]->medID > newMed->medID){
                Node->keys[i + 1] = Node->keys[i];
                i--;
            }
            Node->keys[i + 1] = newMed;
            Node->noOfKeys++;
            *newRight = NULL;
        }
        else {
            MedNode* temp[ORDER];
            for (int i = 0; i < ORDER - 1; i++){
                temp[i] = Node->keys[i];
            }
            int i = Node->noOfKeys - 1;
            while ( i >= 0 && Node->keys[i]->medID > newMed->medID){
                temp[i + 1] = temp[i];
                i--;
            }
            temp[i + 1] = newMed;

            *upkey = temp[ORDER / 2];

            Node->noOfKeys = 0;
            int j;
            for (j = 0; j < ORDER / 2; j++){
                Node->keys[j] = temp[j];
                Node->noOfKeys++;
            }
            BT_MedNode* right = CREATE_NODE(BT_MedNode, true);
            j++;
            int k;
            for (k = 0, j = ORDER / 2 + 1; j < ORDER; k++, j++){
                right->keys[k] = temp[j];
                right->noOfKeys++;
            }
            *newRight = right;
        }
    }
    else {
        int idx = 0;
        while (idx < Node->noOfKeys && Node->keys[idx]->medID < newMed->medID){
            idx++;
        }
        Node->children[idx] = insertInternal_Med(Node->children[idx], newMed, upkey,  newRight);
        if (*newRight != NULL){
            if (Node->noOfKeys < ORDER - 1){
                int i = Node->noOfKeys - 1;
                while (i >= idx){
                    Node->keys[i + 1] = Node->keys[i];
                    Node->children[i + 2] = Node->children[i + 1];
                    i--;
                }
                Node->keys[i + 1] = *upkey;
                Node->noOfKeys++;
                Node->children[i + 2] = *newRight; 
                *newRight = NULL;
            }
            else{
                MedNode* tempKeys[ORDER];
                BT_MedNode* tempChild[ORDER + 1];
                int j;

                for(j = 0 ; j < Node->noOfKeys; j++)
                    tempKeys[j] = Node->keys[j];
                for (j = 0; j < Node->noOfKeys + 1 ; j++)
                    tempChild[j] = Node->children[j];
                for (j = Node->noOfKeys - 1; j >= idx; j-- )
                    tempKeys[j + 1] = tempKeys[j];
                    
                tempKeys[idx] = *upkey;

                for (j = Node->noOfKeys; j >= idx + 1; j--)
                    tempChild[j + 1] = tempChild[j];

                tempChild[idx + 1] = *newRight;
                
                *upkey = tempKeys[ORDER / 2];

                Node->noOfKeys = 0;
                for (j = 0; j < ORDER / 2; j++){
                    Node->keys[j] = tempKeys[j];
                    Node->children[j] = tempChild[j];
                    Node->noOfKeys++;
                }
                Node->children[j] = tempChild[j];

                BT_MedNode* right = CREATE_NODE(BT_MedNode, false);
                right->noOfKeys = 0;
                int rgtIdx;
                for (j = 0, rgtIdx = ORDER / 2 + 1; j < (ORDER - 1) / 2; j++, rgtIdx++) {
                    right->keys[j] = tempKeys[rgtIdx];
                    right->children[j] = tempChild[rgtIdx];
                    right->noOfKeys++;
                }
                right->children[j] = tempChild[rgtIdx];
                *newRight = right;
            }
        }
        ret_Node = Node;
    }
    return ret_Node;
}

void insert_BatchNode(BT_BatchNode** BatchRoot, BatchNode* newBatch){
    BatchNode* upkey;
    if (!(*BatchRoot)){
        (*BatchRoot) = CREATE_NODE(BT_BatchNode, true);
        (*BatchRoot)->keys[0] = newBatch;
        (*BatchRoot)->noOfKeys = 1;
    }
    else {
        BT_BatchNode* newRight = NULL;
        (*BatchRoot) = insertInternal_Batch((*BatchRoot), newBatch, &upkey, &newRight);
        if (newRight != NULL){
            BT_BatchNode* newNode = CREATE_NODE(BT_BatchNode, false);
            newNode->keys[0] = upkey;
            newNode->children[0] = (*BatchRoot);
            newNode->children[1] = newRight;
            newNode->noOfKeys = 1;
            (*BatchRoot) = newNode;
        }
    }
}

BT_BatchNode* insertInternal_Batch(BT_BatchNode* Node, BatchNode* newBatch, BatchNode** upkey, BT_BatchNode** newRight){
    BT_BatchNode* ret_Node = Node;
    if (Node == NULL) return ret_Node;
    if (Node->isLeaf){
        if(Node->noOfKeys < ORDER - 1){
            int i = Node->noOfKeys - 1;
            while (i >= 0 && Node->keys[i]->expiryDate > newBatch->expiryDate){
                Node->keys[i + 1] = Node->keys[i];
                i--;
            }
            Node->keys[i + 1] = newBatch;
            Node->noOfKeys++;
            *newRight = NULL;
        }
        else {
            BatchNode* temp[ORDER];
            for (int i = 0; i < ORDER - 1; i++){
                temp[i] = Node->keys[i];
            }
            int i = Node->noOfKeys - 1;
            while ( i >= 0 && Node->keys[i]->expiryDate > newBatch->expiryDate){
                temp[i + 1] = temp[i];
                i--;
            }
            temp[i + 1] = newBatch;

            *upkey = temp[ORDER / 2];

            Node->noOfKeys = 0;
            int j;
            for (j = 0; j < ORDER / 2; j++){
                Node->keys[j] = temp[j];
                Node->noOfKeys++;
            }
            BT_BatchNode* right = CREATE_NODE(BT_BatchNode, true);
            j++;
            int k;
            for (k = 0, j = ORDER / 2 + 1; j < ORDER; k++, j++){
                right->keys[k] = temp[j];
                right->noOfKeys++;
            }
            *newRight = right;
        }
    }
    else {
        int idx = 0;
        while (idx < Node->noOfKeys && Node->keys[idx]->expiryDate < newBatch->expiryDate)
            idx++;

        Node->children[idx] = insertInternal_Batch(Node->children[idx], newBatch, upkey,  newRight);
        if (*newRight != NULL){
            if (Node->noOfKeys < ORDER - 1){
                int i = Node->noOfKeys - 1;
                while (i >= idx){
                    Node->keys[i + 1] = Node->keys[i];
                    Node->children[i + 2] = Node->children[i + 1];
                    i--;
                }
                Node->keys[i + 1] = *upkey;
                Node->noOfKeys++;
                Node->children[i + 2] = *newRight; 
                *newRight = NULL;
            }
            else{
                BatchNode* tempKeys[ORDER];
                BT_BatchNode* tempChild[ORDER + 1];
                int j;

                for(j = 0 ; j < Node->noOfKeys; j++)
                    tempKeys[j] = Node->keys[j];
                for (j = 0; j < Node->noOfKeys + 1 ; j++)
                    tempChild[j] = Node->children[j];
                for (j = Node->noOfKeys - 1; j >= idx; j-- )
                    tempKeys[j + 1] = tempKeys[j];
                    
                tempKeys[idx] = *upkey;

                for (j = Node->noOfKeys; j >= idx + 1; j--)
                    tempChild[j + 1] = tempChild[j];

                tempChild[idx + 1] = *newRight;
                
                *upkey = tempKeys[ORDER / 2];

                Node->noOfKeys = 0;
                for (j = 0; j < ORDER / 2; j++){
                    Node->keys[j] = tempKeys[j];
                    Node->children[j] = tempChild[j];
                    Node->noOfKeys++;
                }
                Node->children[j] = tempChild[j];

                BT_BatchNode* right = CREATE_NODE(BT_BatchNode, false);
                right->noOfKeys = 0;
                int rgtIdx;
                for (j = 0, rgtIdx = ORDER / 2 + 1; j < (ORDER - 1) / 2; j++, rgtIdx++) {
                    right->keys[j] = tempKeys[rgtIdx];
                    right->children[j] = tempChild[rgtIdx];
                    right->noOfKeys++;
                }
                right->children[j] = tempChild[rgtIdx];
                *newRight = right;
            }
        }
        ret_Node = Node;
    }
    return ret_Node;
}


void insert_SuppNode(BT_SuppNode** suppRoot, SuppNode* newSupp){
    SuppNode* upkey;
    if (!(*suppRoot)){
        (*suppRoot) = CREATE_NODE(BT_SuppNode, true);
        (*suppRoot)->keys[0] = newSupp;
        (*suppRoot)->noOfKeys = 1;
    }
    else {
        BT_SuppNode* newRight = NULL;
        (*suppRoot) = insertInternal_Supp((*suppRoot), newSupp, &upkey, &newRight);
        if (newRight != NULL){
            BT_SuppNode* newNode = CREATE_NODE(BT_SuppNode, false);
            newNode->keys[0] = upkey;
            newNode->children[0] = *suppRoot;
            newNode->children[1] = newRight;
            newNode->noOfKeys = 1;
            *suppRoot = newNode;
        }
    }
}

BT_SuppNode* insertInternal_Supp(BT_SuppNode* Node, SuppNode* newSupp, SuppNode** upkey, BT_SuppNode** newRight){
    BT_SuppNode* ret_Node = Node;
    if (Node == NULL) return ret_Node;
    if (Node->isLeaf){
        if(Node->noOfKeys < ORDER - 1){
            int i = Node->noOfKeys - 1;
            while (i >= 0 && Node->keys[i]->supplierID > newSupp->supplierID){
                Node->keys[i + 1] = Node->keys[i];
                i--;
            }
            Node->keys[i + 1] = newSupp;
            Node->noOfKeys++;
            *newRight = NULL;
        }
        else {
            SuppNode* temp[ORDER];
            for (int i = 0; i < ORDER - 1; i++){
                temp[i] = Node->keys[i];
            }
            int i = Node->noOfKeys - 1;
            while ( i >= 0 && Node->keys[i]->supplierID > newSupp->supplierID){
                temp[i + 1] = temp[i];
                i--;
            }
            temp[i + 1] = newSupp;

            *upkey = temp[ORDER / 2];

            Node->noOfKeys = 0;
            int j;
            for (j = 0; j < ORDER / 2; j++){
                Node->keys[j] = temp[j];
                Node->noOfKeys++;
            }
            BT_SuppNode* right = CREATE_NODE(BT_SuppNode, true);
            j++;
            int k;
            for (k = 0, j = ORDER / 2 + 1; j < ORDER; k++, j++){
                right->keys[k] = temp[j];
                right->noOfKeys++;
            }
            *newRight = right;
        }
    }
    else {
        int idx = 0;
        while (idx < Node->noOfKeys && Node->keys[idx]->supplierID < newSupp->supplierID){
            idx++;
        }
        Node->children[idx] = insertInternal_Supp(Node->children[idx], newSupp, upkey,  newRight);
        if (*newRight != NULL){
            if (Node->noOfKeys < ORDER - 1){
                int i = Node->noOfKeys - 1;
                while (i >= idx){
                    Node->keys[i + 1] = Node->keys[i];
                    Node->children[i + 2] = Node->children[i + 1];
                    i--;
                }
                Node->keys[i + 1] = *upkey;
                Node->noOfKeys++;
                Node->children[i + 2] = *newRight; 
                *newRight = NULL;
            }
            else{
                SuppNode* tempKeys[ORDER];
                BT_SuppNode* tempChild[ORDER + 1];
                int j;

                for(j = 0 ; j < Node->noOfKeys; j++)
                    tempKeys[j] = Node->keys[j];
                for (j = 0; j < Node->noOfKeys + 1 ; j++)
                    tempChild[j] = Node->children[j];
                for (j = Node->noOfKeys - 1; j >= idx; j-- )
                    tempKeys[j + 1] = tempKeys[j];
                    
                tempKeys[idx] = *upkey;

                for (j = Node->noOfKeys; j >= idx + 1; j--)
                    tempChild[j + 1] = tempChild[j];

                tempChild[idx + 1] = *newRight;
                
                *upkey = tempKeys[ORDER / 2];

                Node->noOfKeys = 0;
                for (j = 0; j < ORDER / 2; j++){
                    Node->keys[j] = tempKeys[j];
                    Node->children[j] = tempChild[j];
                    Node->noOfKeys++;
                }
                Node->children[j] = tempChild[j];

                BT_SuppNode* right = CREATE_NODE(BT_SuppNode, false);
                right->noOfKeys = 0;
                int rgtIdx;
                for (j = 0, rgtIdx = ORDER / 2 + 1; j < (ORDER - 1) / 2; j++, rgtIdx++) {
                    right->keys[j] = tempKeys[rgtIdx];
                    right->children[j] = tempChild[rgtIdx];
                    right->noOfKeys++;
                }
                right->children[j] = tempChild[rgtIdx];
                *newRight = right;
            }
        }
        ret_Node = Node;
    }
    return ret_Node;
}

/* Search Functions */

BT_MedNode* search_MedNode(BT_MedNode* Node, int medID){
    BT_MedNode* ret_Node = NULL;
    if (Node != NULL){
        int i = 0;
        while (i < Node->noOfKeys && Node->keys[i]->medID < medID) i++;
        if (i < Node->noOfKeys && Node->keys[i]->medID == medID) ret_Node = Node;
        else if (Node->isLeaf) ret_Node = NULL;
        else ret_Node = search_MedNode(Node->children[i], medID);
    }
    return ret_Node;
}
MedNode* search_MedNode_ByID(BT_MedNode* Node, int medID){
    if (!Node) return NULL;  // Added NULL check
    BT_MedNode* mNode = search_MedNode(Node, medID);
    if (!mNode) return NULL;
    
    for (int i = 0; i < mNode->noOfKeys; i++){
        if (mNode->keys[i]->medID == medID)
            return mNode->keys[i];
    }
    return NULL;
}

BT_SuppNode* search_SuppNode (BT_SuppNode* Node, int suppID){
    BT_SuppNode* ret_Node = NULL;
    if (Node != NULL){
        int i = 0;
        while (i < Node->noOfKeys && Node->keys[i]->supplierID < suppID) i++;
        if (i < Node->noOfKeys && Node->keys[i]->supplierID == suppID) ret_Node = Node;
        else if (Node->isLeaf) ret_Node = NULL;
        else ret_Node = search_SuppNode(Node->children[i], suppID);
    }
    return ret_Node;
}
SuppNode* search_SuppNode_ByID (BT_SuppNode* Node, int suppID){
    if (!Node) return NULL;  // Added NULL check
    BT_SuppNode* sNode = search_SuppNode(Node, suppID);
    if (!sNode) return NULL;
    
    for (int i = 0; i < sNode->noOfKeys; i++){
        if (sNode->keys[i]->supplierID == suppID)
            return sNode->keys[i];
    }
    return NULL;
}




// B-Tree operations for suppliers
BT_SuppNode* createSuppBTreeNode(bool isLeaf) {
    BT_SuppNode* newNode = (BT_SuppNode*)malloc(sizeof(BT_SuppNode));
    if (!newNode) {
        printf("Memory allocation failed for B-Tree node\n");
        exit(EXIT_FAILURE);
    }
    newNode->noOfKeys = 0;
    newNode->isLeaf = isLeaf;
    for (int i = 0; i < ORDER; i++) {
        newNode->children[i] = NULL;
    }
    return newNode;
}

void traverseSuppBTree(BT_SuppNode* root) {
    if (root) {
        for (int i = 0; i < root->noOfKeys; i++) {
            if (!root->isLeaf) {
                traverseSuppBTree(root->children[i]);
            }
            printf("Supplier ID: %d, Name: %s\n", 
                   root->keys[i]->supplierID, 
                   root->keys[i]->supplierName);
        }
        if (!root->isLeaf) {
            traverseSuppBTree(root->children[root->noOfKeys]);
        }
    }
}

SuppNode* searchSuppBTree(BT_SuppNode* root, int supplierID) {
    int i = 0;
    while (i < root->noOfKeys && supplierID > root->keys[i]->supplierID) {
        i++;
    }
    
    if (i < root->noOfKeys && supplierID == root->keys[i]->supplierID) {
        return root->keys[i];
    }
    
    if (root->isLeaf) {
        return NULL;
    }
    
    return searchSuppBTree(root->children[i], supplierID);
}

void splitSuppChild(BT_SuppNode* parent, int index) {
    BT_SuppNode* child = parent->children[index];
    BT_SuppNode* newChild = createSuppBTreeNode(child->isLeaf);
    newChild->noOfKeys = ORDER / 2 - 1;
    
    // Copy the right half of child to newChild
    for (int i = 0; i < ORDER / 2 - 1; i++) {
        newChild->keys[i] = child->keys[i + ORDER / 2];
    }
    
    if (!child->isLeaf) {
        for (int i = 0; i < ORDER / 2; i++) {
            newChild->children[i] = child->children[i + ORDER / 2];
        }
    }
    
    child->noOfKeys = ORDER / 2 - 1;
    
    // Shift parent's children to make space for newChild
    for (int i = parent->noOfKeys; i > index; i--) {
        parent->children[i + 1] = parent->children[i];
    }
    parent->children[index + 1] = newChild;
    
    // Shift parent's keys and insert the middle key from child
    for (int i = parent->noOfKeys - 1; i >= index; i--) {
        parent->keys[i + 1] = parent->keys[i];
    }
    parent->keys[index] = child->keys[ORDER / 2 - 1];
    parent->noOfKeys++;
}

void insertSuppNonFull(BT_SuppNode* node, SuppNode* supplier) {
    int i = node->noOfKeys - 1;
    
    if (node->isLeaf) {
        // Find location to insert new key
        while (i >= 0 && supplier->supplierID < node->keys[i]->supplierID) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = supplier;
        node->noOfKeys++;
    } else {
        // Find child which will have the new key
        while (i >= 0 && supplier->supplierID < node->keys[i]->supplierID) {
            i--;
        }
        i++;
        
        // Check if child is full
        if (node->children[i]->noOfKeys == ORDER - 1) {
            splitSuppChild(node, i);
            if (supplier->supplierID > node->keys[i]->supplierID) {
                i++;
            }
        }
        insertSuppNonFull(node->children[i], supplier);
    }
}

void insertSupplier(BT_SuppNode** root, SuppNode* supplier) {
    BT_SuppNode* oldRoot = *root;
    
    if (oldRoot->noOfKeys == ORDER - 1) {
        BT_SuppNode* newRoot = createSuppBTreeNode(false);
        newRoot->children[0] = oldRoot;
        *root = newRoot;
        splitSuppChild(newRoot, 0);
        insertSuppNonFull(newRoot, supplier);
    } else {
        insertSuppNonFull(oldRoot, supplier);
    }
}

// Supplier management functions using B-Tree
void addSupplierToMedication(BT_SuppNode** suppRoot, MedNode* med) {
    SuppNode* newSupp = (SuppNode*)malloc(sizeof(SuppNode));
    for (int i = 0; i < MAX_MED_IN_SUPP; i++) {
        newSupp->MedPtr[i] = NULL;
    }
    if (!newSupp) {
        printf("Memory allocation failed for new supplier\n");
        return;
    }
    
    printf("\nEnter Supplier ID: ");
    scanf("%d", &newSupp->supplierID);
    getchar();
    
    printf("Enter Supplier Name: ");
    fgets(newSupp->supplierName, NAME_LEN, stdin);
    newSupp->supplierName[strcspn(newSupp->supplierName, "\n")] = '\0';
    
    printf("Enter Contact Info: ");
    fgets(newSupp->contactInfo, NUM_LEN, stdin);
    newSupp->contactInfo[strcspn(newSupp->contactInfo, "\n")] = '\0';
    
    printf("Enter Quantity Supplied: ");
    scanf("%d", &newSupp->quantitySupplied);
    
    newSupp->uniqueMedication = 1;
    newSupp->totalCostSupplied = med->pricePerUnit * newSupp->quantitySupplied;
    
    for (int i = 0; i < MAX_MED_IN_SUPP; i++) {
        if (newSupp->MedPtr[i] == NULL) {
            newSupp->MedPtr[i] = med;
            break;
        }
        if (i == MAX_MED_IN_SUPP - 1) {
            printf("Supplier cannot handle more medications!\n");
            free(newSupp);
            return;
        }
    }
    
    // Add to B-Tree
    insertSupplier(suppRoot, newSupp);
    
    // Also add to medication's linked list (for quick access from medication)
    newSupp->next = med->suppliers;
    med->suppliers = newSupp;
    
    printf("Supplier added successfully.\n");
}

void updateSupplierInMedication(BT_SuppNode* suppRoot, MedNode* med) {
    int suppID;
    printf("\nEnter Supplier ID to update: ");
    scanf("%d", &suppID);
    
    SuppNode* foundSupp = searchSuppBTree(suppRoot, suppID);
    if (!foundSupp) {
        printf("Supplier with ID %d not found.\n", suppID);
        return;
    }
    
    printf("\nCurrent Supplier Details:");
    printf("\n1. Name: %s", foundSupp->supplierName);
    printf("\n2. Contact: %s", foundSupp->contactInfo);
    printf("\n3. Quantity Supplied: %d", foundSupp->quantitySupplied);
    
    printf("\n\nWhat do you want to update?");
    printf("\n1. Name\n2. Contact\n3. Quantity Supplied\nChoice: ");
    int choice;
    scanf("%d", &choice);
    getchar();
    
    switch(choice) {
        case 1:
            printf("\nEnter new name: ");
            fgets(foundSupp->supplierName, NAME_LEN, stdin);
            foundSupp->supplierName[strcspn(foundSupp->supplierName, "\n")] = '\0';
            break;
        case 2:
            printf("\nEnter new contact: ");
            fgets(foundSupp->contactInfo, NUM_LEN, stdin);
            foundSupp->contactInfo[strcspn(foundSupp->contactInfo, "\n")] = '\0';
            break;
        case 3: {
            int newQty;
            printf("\nEnter new quantity supplied: ");
            scanf("%d", &newQty);
            foundSupp->totalCostSupplied += med->pricePerUnit * (newQty - foundSupp->quantitySupplied);
            foundSupp->quantitySupplied = newQty;
            break;
        }
        default:
            printf("\nInvalid choice.");
    }
    
    printf("\nSupplier updated successfully.\n");
}

void deleteSupplierFromMedication(BT_SuppNode** suppRoot, MedNode* med) {
    int suppID;
    printf("\nEnter Supplier ID to delete: ");
    scanf("%d", &suppID);
    
    // First remove from medication's linked list
    SuppNode *prev = NULL, *curr = med->suppliers;
    while (curr && curr->supplierID != suppID) {
        prev = curr;
        curr = curr->next;
    }
    
    if (!curr) {
        printf("Supplier with ID %d not found for this medication.\n", suppID);
        return;
    }
    
    if (prev) {
        prev->next = curr->next;
    } else {
        med->suppliers = curr->next;
    }
    
    // Then remove from B-Tree (simplified - in a real implementation, you'd need a proper B-Tree deletion)
    // For now, we'll just mark it as deleted in the B-Tree by setting supplierID to -1
    SuppNode* foundSupp = searchSuppBTree(*suppRoot, suppID);
    if (foundSupp) {
        foundSupp->supplierID = -1;  // Mark as deleted
    }
    
    free(curr);
    printf("Supplier deleted successfully.\n");
}

void searchSupplierInMedication(BT_SuppNode* suppRoot) {
    int suppID;
    printf("\nEnter Supplier ID to search: ");
    scanf("%d", &suppID);
    
    SuppNode* foundSupp = searchSuppBTree(suppRoot, suppID);
    if (!foundSupp) {
        printf("Supplier with ID %d not found.\n", suppID);
        return;
    }
    
    printf("\nSupplier Details:");
    printf("\nID: %d", foundSupp->supplierID);
    printf("\nName: %s", foundSupp->supplierName);
    printf("\nContact: %s", foundSupp->contactInfo);
    printf("\nQuantity Supplied: %d", foundSupp->quantitySupplied);
    printf("\nTotal Cost Supplied: %.2f", foundSupp->totalCostSupplied);
    
    for (int i = 0; i < MAX_MED_IN_SUPP && foundSupp->MedPtr[i] != NULL; i++) {
        printf("Medication detail %d:\n", i+1);
        printf("Medication ID: %d \n", foundSupp->MedPtr[i]->medID);
    }
}


void collectSuppliers(BT_SuppNode* node, SuppNode** suppliers, int* count) {
    if (node) {
        for (int i = 0; i < node->noOfKeys; i++) {
            if (!node->isLeaf) {
                collectSuppliers(node->children[i], suppliers, count);
            }
            suppliers[*count] = node->keys[i];
            (*count)++;
        }
        if (!node->isLeaf) {
            collectSuppliers(node->children[node->noOfKeys], suppliers, count);
        }
    }
}

int compareUniqueMed(const void* a, const void* b) {
    SuppNode* suppA = *(SuppNode**)a;
    SuppNode* suppB = *(SuppNode**)b;
    return suppB->uniqueMedication - suppA->uniqueMedication;
}

int compareTurnover(const void* a, const void* b) {
    SuppNode* suppA = *(SuppNode**)a;
    SuppNode* suppB = *(SuppNode**)b;
    return (suppB->totalCostSupplied > suppA->totalCostSupplied) ? 1 : -1;
}
