#include "prototypes.h"

MedHash* MedTable[TABLE_SIZE];

int main(){
    
    BT_roots* Trees = NULL;
    initialisation(&Trees);
    loadDataFromFile(Trees);

    int choice;
    do {
        printf("\n\n===== Pharmacy Inventory Management =====\n");
        printf("\n1. Add New Medication\n2. Update Medication\n3. Delete Medication\n4. Search Medication");
        printf("\n5. Stock Alerts\n6. Check Expiration Dates\n7. Check Medication between dates\n8. Record Sales");
        printf("\n9. Supplier Management:\n\tAdd, update, delete, and Search");
        printf("\n10. All-Rounder Suppliers\n11. Largest Turnover Suppliers\n0. Exit\n\nSelect an option: ");
        scanf("%d", &choice);
        while (getchar() != '\n'); 

        switch(choice) {
            case 1: addNewMedication(Trees); break;
            case 2: updateMedication(Trees); break;
            case 3: deleteMedication(Trees); break;
            case 4: searchMedication(Trees); break;
            case 5: stockAlerts(Trees); break;
            case 6: checkExpirationDates(Trees); break;
            case 7: MedBetweenDates(Trees); break;
            case 8: recordSales(Trees); break;
            case 9: supplierManagement(Trees); break;
            case 10: FindAllRounderSupplier(Trees->SupplierTree); break;
            case 11: FindLargestTuroverSupplier(Trees->SupplierTree); break;
            case 0: {
                saveDataToFile(Trees);
                freeResources(Trees);
                printf("\nExiting...\n");
                break;
            }
            default: printf("\nInvalid choice!");
        }
    } while(choice != 0);

    return 0;
}