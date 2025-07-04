# 💊 Pharmacy Inventory Management System

This is a **C-based Pharmacy Inventory Management System** designed to handle medication, batch tracking, supplier management, stock alerts, and expiration tracking using B-Trees for efficient storage and retrieval. It provides a terminal interface for managing pharmacy data with features like file handling, adding and searching datas, and more.

---

## 📁 Project Structure

```
.
├── main.c                  # Entry point with menu-based UI
├── mainFn.c               # Core pharmacy functionalities (CRUD, sales, etc.)
├── helpingFn.c            # Utility and B-Tree helper functions
├── supportiveFn.c         # Generic B-Tree implementation and hashing
├── structures.h           # Struct definitions for MedNode, BatchNode, SuppNode, B-Trees
├── prototypes.h           # All function declarations
├── Makefile               # Build automation file
├── pharma.txt             # Data file used for persistence (auto-created)
```

---

## ✅ Features

- **Medication Management**
  - Add, update, delete, search medications
  - Track reorder levels and total sales
  - Batches linked to medications with stock and expiry

- **Batch Management**
  - Store batches in a B-Tree sorted by expiration date
  - Track expiry alerts and range-based expiry queries

- **Supplier Management**
  - Add, update, delete, and search suppliers
  - Link medications to suppliers with quantity and cost tracking

- **Hashing**
  - Fast lookup for medication name using custom hash table

- **File Persistence**
  - Save/load data to/from `pharma.txt` automatically

- **Efficient Search**
  - All trees (Medications, Batches, Suppliers) implemented using generic B-Trees

- **Reports**
  - Stock alert report
  - Expiry date warnings
  - Range-based expiry listing
  - All-rounder suppliers (most unique medications)
  - Top suppliers by turnover

---

## ⚙️ Build Instructions

### Prerequisites
- GCC or any C compiler
- `make` utility installed

### To Build:
```bash
make
```

### To Run:
```bash
./Pharmacy_Management
```


## 📄 Data File Format (pharma.txt)

```txt
#MEDICATION_BTREE
MEDICATION 201, Paracetamol, 1.50, 50, 120, 300
BATCH B101, 20261201, 70, 180
BATCH B102, 20261215, 50, 120
END_MEDICATION
...
#SUPPLIER_BTREE
SUPPLIER_TREE_NODE 101, HealthPharma, 9876543210, 2, 12500.50, 201 100, 202 50
...
```

This format is auto-generated and auto-loaded upon startup of the program.
