#include "prototypes.h"

void BT_insert(BTree* tree, void* key){
    BTreeNode* head = tree->root;
    void* upkey;
    if (!head){
        head = createBT_Node(true);
        head->keys[0] = key;
        head->noOfKeys++;
    }
    else {
        BTreeNode* newRight = NULL;
        head = BT_insertInternal(tree, head, key, &upkey, &newRight);
        if (newRight != NULL){
            BTreeNode* newNode = createBT_Node(false);
            newNode->keys[0] = upkey;
            newNode->children[0] = head;
            newNode->children[1] = newRight;
            newNode->noOfKeys = 1;
            head = newNode;
        }
    }
    tree->root = head;
}

BTreeNode* BT_insertInternal(BTree* tree, BTreeNode* Node, void* key, void** upkey, BTreeNode** newRight){
    if (Node == NULL) return NULL;
    BTreeNode* ret_Node = Node;
    if (Node->isLeaf){
        if(Node->noOfKeys < ORDER - 1){
            int i = Node->noOfKeys - 1;
            while (i >= 0 && tree->cmp(Node->keys[i], key) > 0){
                Node->keys[i + 1] = Node->keys[i];
                i--;
            }
            Node->keys[i + 1] = key;
            Node->noOfKeys++;
            *newRight = NULL;
        } 
        else {
            void* temp[ORDER];
            for (int i = 0; i < ORDER - 1; i++){
                temp[i] = Node->keys[i];
            }
            int i = Node->noOfKeys - 1;
            while ( i >= 0 && tree->cmp(Node->keys[i], key) > 0){
                temp[i + 1] = temp[i];
                i--;
            }
            temp[i + 1] = key;

            *upkey = temp[ORDER / 2];

            Node->noOfKeys = 0;
            int j;
            for (j = 0; j < ORDER / 2; j++){
                Node->keys[j] = temp[j];
                Node->noOfKeys++;
            }
            BTreeNode* right = createBT_Node(true);
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
        while (idx < Node->noOfKeys && tree->cmp(Node->keys[idx], key) < 0){
            idx++;
        }
        Node->children[idx] = BT_insertInternal(tree, Node->children[idx], key, upkey,  newRight);
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
                void* tempKeys[ORDER];
                BTreeNode* tempChild[ORDER + 1];
                int j;

                for(j = 0 ; j < Node->noOfKeys; j++){
                    tempKeys[j] = Node->keys[j];
                    tempChild[j] = Node->children[j];
                }
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

                BTreeNode* right = createBT_Node(false);
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

void insertIn_Med_Hash(char* medName, int medID){
    MedHash* newEntry = (MedHash*)malloc(sizeof(MedHash));
    if (!newEntry) {
        printf("Memory allocation failed\n");
        return;
    }
    char lowerName[100];
    toLowerStr(lowerName, medName);

    strcpy(newEntry->medName, lowerName);
    newEntry->medID = medID;
    newEntry->next = NULL;

    int idx = MedKey(lowerName);
    newEntry->next = MedTable[idx];
    MedTable[idx] = newEntry;
}

unsigned int MedKey(const char* medName){
    unsigned int hash = 101;
    while(*medName != '\0'){
        hash = hash * 31 + (unsigned int)(*medName);
        medName++;
    }
    return hash % TABLE_SIZE;
}

int searchIn_Med_Hash(char* medName){
    char lowerName[100];
    toLowerStr(lowerName, medName);

    int idx = MedKey(lowerName);
    MedHash* curr = MedTable[idx];
    while(curr != NULL){
        if(strcmp(curr->medName, lowerName) == 0){
            return curr->medID;
        }
        curr = curr->next;
    }
    return 0;
}

void* search_ByID(BTree* tree, void* ID) {
    BTreeNode* node = tree->root;

    while (node) {
        int i = 0;
        while (i < node->noOfKeys && tree->cmp(node->keys[i], ID) < 0)
            i++;

        if (i < node->noOfKeys && tree->cmp(node->keys[i], ID) == 0)
            return node->keys[i];

        if (node->isLeaf) return NULL;

        node = node->children[i];
    }

    return NULL;
}

BatchNode* search_BatchNode_ByBatchNo(BTreeNode* Node, char* batchNo) {
    if (!Node) return NULL;

    int i;
    for (i = 0; i < Node->noOfKeys; i++) {
        if (!Node->isLeaf) {
            BatchNode* found = search_BatchNode_ByBatchNo(Node->children[i], batchNo);
            if (found) return found;
        }
        BatchNode* bt = (BatchNode*)Node->keys[i];
        if (strcmp(bt->batchNo, batchNo) == 0) return bt;
    }

    if (!Node->isLeaf) {
        BatchNode* found = search_BatchNode_ByBatchNo(Node->children[i], batchNo);
        if (found) return found;
    }

    return NULL;
}

void traverse_StockAlerts(BTreeNode* mNode, int* found){
    if(!mNode) return;
    int i = 0;
    for(; i < mNode->noOfKeys; i++){
        MedNode* med = (MedNode*)mNode->keys[i];
        if(!(mNode->isLeaf)) traverse_StockAlerts(mNode->children[i], found);
        if(med->stockQuantity <= med->reorderLevel){
            printf("\n| [%d] %s | %.2f | %d |", med->medID, med->medName, med->pricePerUnit, med->stockQuantity);
            *found = 1;
        }
    }
    if(!(mNode->isLeaf)) traverse_StockAlerts(mNode->children[i], found);
}

void traverse_ExpirationDates(BTreeNode* bNode, int today, int* found){
    if(!bNode) return;
    int i = 0;
    for(; i < bNode->noOfKeys; i++){
        BatchNode* batch = (BatchNode*)bNode->keys[i];
        if(!(bNode->isLeaf)) traverse_ExpirationDates(bNode->children[i], today, found);
        if(batch->expiryDate - today <= 30){
            printf("\n| [%d] %s\t| %s\t| %d\t| %s\t|", batch->medPtr->medID, batch->medPtr->medName, batch->batchNo, batch->stock, format_DATE(batch->expiryDate));
            *found = 1;
        }
    }
    if(!(bNode->isLeaf)) traverse_ExpirationDates(bNode->children[i], today, found);
}

void rangeSearch_Dates(BTreeNode* node, int date1, int date2, int* found){
    int i = -1;
    BatchNode* batch;
    do{
        i++;
        batch = (BatchNode*)node->keys[i];
    }
    while(i < node->noOfKeys && batch->expiryDate < date1);

    for(; i < node->noOfKeys && batch->expiryDate <= date2; i++){
        batch = (BatchNode*)node->keys[i];
        if(!node->isLeaf) rangeSearch_Dates(node->children[i], date1, date2, found);
        if(batch->expiryDate >= date1){
            printf("\n| [%d] %s\t| %s\t| %d\t| %s|", batch->medPtr->medID, batch->medPtr->medName, batch->batchNo, batch->stock, format_DATE(batch->expiryDate));
            *found = 1;
        }
    }
    if(!node->isLeaf) rangeSearch_Dates(node->children[i], date1, date2, found);
}

/* ------------------------ Deletion Functions ------------------------ */

void BT_Delete(BTree* Tree, void* k) {
    if (Tree->root == NULL) {
        printf("The tree is empty\n");
    } else {
        deleteKey(Tree, Tree->root, k);
        if (Tree->root->noOfKeys == 0) {
            BTreeNode* tmp = Tree->root;
            if (Tree->root->isLeaf) Tree->root = NULL;
            else Tree->root = Tree->root->children[0];
            free(tmp);
        }
    }
}

void deleteKey(BTree* Tree, BTreeNode* node, void* key) {
    int idx = 0;
    while (idx < node->noOfKeys && Tree->cmp(node->keys[idx], key) < 0)
        idx++;

    if (idx < node->noOfKeys && Tree->cmp(node->keys[idx], key) == 0) {
        if (node->isLeaf) removeFromLeaf(node, idx);
        else removeFromNonLeaf(Tree, node, idx);
    } 
    else {
        if (node->isLeaf)
            printf("\nThe key does not exist in the tree");
        else {
            bool flag = (idx == node->noOfKeys);
            if (node->children[idx]->noOfKeys < (ORDER - 1) / 2)
                fill(node, idx);
            if (flag && idx > node->noOfKeys)
                deleteKey(Tree, node->children[idx - 1], key);
            else
                deleteKey(Tree, node->children[idx], key);
        }
    }
}

void removeFromLeaf(BTreeNode* node, int idx) {
    int i;
    for (i = idx + 1; i < node->noOfKeys; i++)
        node->keys[i - 1] = node->keys[i];
    node->noOfKeys--;
}

void removeFromNonLeaf(BTree* Tree, BTreeNode* node, int idx) {
    if (node->children[idx]->noOfKeys >= (ORDER - 1) / 2) {
        void* pred = getPred(node, idx);
        node->keys[idx] = pred;
        deleteKey(Tree, node->children[idx], pred);
    } else if (node->children[idx + 1]->noOfKeys >= (ORDER - 1) / 2) {
        void* succ = getSucc(node, idx);
        node->keys[idx] = succ;
        deleteKey(Tree, node->children[idx + 1], succ);
    } else {
        mergeNodes(node, idx);
        deleteKey(Tree, node->children[idx], node->keys[idx]);
    }
}

void* getPred(BTreeNode* node, int idx) {
    BTreeNode* cur = node->children[idx];
    while (!cur->isLeaf)
        cur = cur->children[cur->noOfKeys];
    return cur->keys[cur->noOfKeys - 1];
}

void* getSucc(BTreeNode* node, int idx) {
    BTreeNode* cur = node->children[idx + 1];
    while (!cur->isLeaf)
        cur = cur->children[0];
    return cur->keys[0];
}

void mergeNodes(BTreeNode* node, int idx) {
    BTreeNode* child = node->children[idx];
    BTreeNode* sibling = node->children[idx + 1];
    int i;
    child->keys[child->noOfKeys] = node->keys[idx];
    for (i = 0; i < sibling->noOfKeys; i++)
        child->keys[i + child->noOfKeys + 1] = sibling->keys[i];
    if (!child->isLeaf) {
        for (i = 0; i <= sibling->noOfKeys; i++)
            child->children[i + child->noOfKeys + 1] = sibling->children[i];
    }
    child->noOfKeys = child->noOfKeys + sibling->noOfKeys + 1;
    for (i = idx + 1; i < node->noOfKeys; i++)
        node->keys[i - 1] = node->keys[i];
    for (i = idx + 2; i <= node->noOfKeys; i++)
        node->children[i - 1] = node->children[i];
    node->noOfKeys--;
    free(sibling);
}

void fill(BTreeNode* node, int idx) {
    if (idx != 0 && node->children[idx - 1]->noOfKeys >= 2)
        borrowFromPrev(node, idx);
    else if (idx != node->noOfKeys && node->children[idx + 1]->noOfKeys >= 2)
        borrowFromNext(node, idx);
    else {
        if (idx != node->noOfKeys) mergeNodes(node, idx);
        else mergeNodes(node, idx - 1);
    }
}

void borrowFromPrev(BTreeNode* node, int idx) {
    BTreeNode* child = node->children[idx];
    BTreeNode* sibling = node->children[idx - 1];
    int i;
    for (i = child->noOfKeys - 1; i >= 0; i--)
        child->keys[i + 1] = child->keys[i];
    if (!child->isLeaf) {
        for (i = child->noOfKeys; i >= 0; i--)
            child->children[i + 1] = child->children[i];
    }
    child->keys[0] = node->keys[idx - 1];
    if (!child->isLeaf)
        child->children[0] = sibling->children[sibling->noOfKeys];
    node->keys[idx - 1] = sibling->keys[sibling->noOfKeys - 1];
    child->noOfKeys++;
    sibling->noOfKeys--;
}

void borrowFromNext(BTreeNode* node, int idx) {
    BTreeNode* child = node->children[idx];
    BTreeNode* sibling = node->children[idx + 1];
    child->keys[child->noOfKeys] = node->keys[idx];
    if (!child->isLeaf)
        child->children[child->noOfKeys + 1] = sibling->children[0];
    node->keys[idx] = sibling->keys[0];
    int i;
    for (i = 1; i < sibling->noOfKeys; i++)
        sibling->keys[i - 1] = sibling->keys[i];
    if (!sibling->isLeaf) {
        for (i = 1; i <= sibling->noOfKeys; i++)
            sibling->children[i - 1] = sibling->children[i];
    }
    child->noOfKeys++;
    sibling->noOfKeys--;
}

void collectSuppliers(BTreeNode* node, SuppNode** suppliers, int* count) {
    if (node) {
        for (int i = 0; i < node->noOfKeys; i++) {
            if (!node->isLeaf) {
                collectSuppliers(node->children[i], suppliers, count);
            }
            suppliers[(*count)++] = (SuppNode*)node->keys[i];
        }
        if (!node->isLeaf) {
            collectSuppliers(node->children[node->noOfKeys], suppliers, count);
        }
    }
}