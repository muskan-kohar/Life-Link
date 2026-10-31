# Life-Link – Blood Donation Management System

## About the Project

Life-Link is a blood donation management system developed using C. The main idea behind this project is to maintain donor details and make it easier to search for suitable blood donors based on blood group compatibility.

The program allows users to register donors, search for donors, find compatible blood groups, remove donor records and display all the registered donors.

## How the Project Works

We have used different data structures in this project, with each one handling a particular part of the system.

- **Hash Table:** Organizes donors by blood group, with linked lists for donors in each group.
- **Linked List:** Each hash table bucket contains a linked list, since multiple donors can belong to the same blood group.
- **Graph:** We use an adjacency matrix to represent the compatibility between different blood groups. This helps us find which donor blood groups are compatible with a recipient.
- **Min Heap:** Maintains a distance heap for each blood group as donors are added, removed, or matched. Compatible-group heaps are searched to find the nearest donor who meets the 90-day donation interval.
- **Queue:** Stores emergency requests in first-in, first-out order.
- **Stack:** Stores fulfilled emergency requests, with the most recently completed request shown first.

## Features

The program provides the following operations:

1. Register a new donor by entering their details.
2. Search for donors using their blood group.
3. Find donors compatible with a particular blood group.
4. Display all the registered donors.
5. Remove a donor using their blood group and unique donor ID.
6. Validate donor details such as contact number and donation date.
7. Add emergency requests to a FIFO queue.
8. Process the oldest pending request and record successful matches in request history.
9. View pending emergency requests.
10. View completed request history.

## How to Run the Program

Make sure you have GCC or another C compiler installed.

First, clone the repository:

```bash
git clone <repository-url>
```

Move into the project directory:

```bash
cd Life-Link
```

Compile the program:

```bash
gcc -std=c11 main.c graph.c hashtable.c heap.c emergency.c -o life-link
```

Run the executable:

**Windows:**
```bash
life-link.exe
```

**Linux/macOS:**
```bash
./life-link
```

## Using the Program

When the program starts, a menu is displayed with different operations.

**Register Donor:** Enter the donor's name, blood group, location, distance, last donation date and contact number. A unique donor ID is assigned after successful registration.

**Search Donor:** Enter a blood group to view the donors registered under it.

**Find Compatible Donors:** Enter the recipient's blood group. The program checks the compatibility graph and selects the nearest eligible donor from the maintained heaps for compatible blood groups. Following the project assumption, a selected donor is marked as having donated today and becomes ineligible for another 90 days.

**Display All Donors:** Displays the details of all donors currently registered in the system.

**Remove Donor:** Enter the donor's blood group and unique ID to remove the corresponding record.

**Emergency Requests:** Add a request using the patient's blood group. Processing always checks the oldest pending request first. If an eligible compatible donor is found, the donor's last-donation date is updated to today, the request is removed from the queue, and the match is recorded in completed history. If no eligible donor is found, the request remains pending for a later retry. Pending requests are displayed FIFO, and completed matches are displayed most-recent-first.

A few things to remember while entering donor details:
- Names and locations can contain spaces.
- Contact numbers must contain exactly 10 digits.
- Enter a valid donation date.
- Use `00-00-0000` if the donor has never donated before.

## Project Files

The project is divided into different source and header files:

- `main.c` – Contains the main function and menu operations.
- `donor.h` – Contains the donor structure.
- `graph.c` and `graph.h` – Handle blood group compatibility.
- `hashtable.c` and `hashtable.h` – Handle donor storage, registration, searching and deletion.
- `heap.c` and `heap.h` – Contain the min heap implementation.
- `emergency.c` and `emergency.h` – Manage the emergency request queue and completed-request history stack.

## Limitations

Currently, all donor records are stored in memory, so the records are lost once the program is closed. Donor IDs also restart from 1001 whenever the program is run again.

Emergency requests and their history are also held in memory and are cleared when the program exits. This version models one fixed center: donor distance is entered manually relative to that center, and donor location is descriptive rather than a separate hash-table key. The program does not calculate hospital-specific distance or use GPS. For this project's simulation, finding a donor automatically records a donation on the current date; it does not verify actual donor consent or completion.

The contact number validation is based on the 10-digit Indian mobile number format.

The compatibility system is based on red blood cell transfusion compatibility. Actual transfusions require proper blood grouping, cross-matching and medical verification.

## Technologies Used

- C Programming Language
- Data Structures
- Pointers and Dynamic Memory Allocation
