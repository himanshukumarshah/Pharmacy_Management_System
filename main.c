#include "structures.h"
MedName* MedTable[TABLE_SIZE] = {NULL};

int main(){
    BTree* Trees = (BTree*) malloc(sizeof(BTree));
    if(Trees == NULL){
        printf("Memory allocation for Trees failed.\n");
        return 1;
    }
    initialise_DB(Trees);
    loadDataFromFile(Trees);

    // stockQuantity(medicationList);
    
    // calculateUniqueMedicationCount(medicationList);

    // totalCostSupplied(Trees->Supp_root);

    int choice;
    do {
        printf("\n==== Pharmacy Inventory Management ====");
        printf("\n1. Add New Medication\n2. Update Medication\n3. Delete Medication\n4. Search Medication");
        printf("\n5. Stock Alerts\n6. Check Expiration Dates\n7. Sort by Expiry\n8. Record Sales");
        printf("\n9. Supplier Management: Add, update, delete, and Search for suppliers");
        printf("\n10. All-Rounder Suppliers\n11. Largest Turnover Suppliers\n0. Exit\n\nSelect an option: ");
        scanf("%d", &choice);
        while (getchar() != '\n'); 
        

        switch(choice) {
            case 1: addNewMedication(Trees); break;
            case 2: updateMedication(Trees); break;
            case 3: deleteMedication(Trees); break;
            case 4: searchMedication(Trees); break;
            case 5: stockAlerts(Trees); break;
            case 6: checkExpiration(Trees); break;
            case 7: check_MedicationBet_DATES(Trees); break;
            case 8: recordSales(Trees); break;
            case 9: supplierManagement(Trees); break;
            case 10: findAll_rounderSupplier(Trees); break;
            case 11: findTop10LargestTurnover(Trees); break;
            case 0: {
                saveDataToFile(Trees);
                printf("\nData Saved!\n");
                freeResources(Trees);
                printf("\nExiting...\n");
                break;
            }
            default: printf("\nInvalid choice!");
        }
    } while(choice != 0);
    return 0;
}