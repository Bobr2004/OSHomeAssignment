# Lab Report

**Student:** Шовкопляс Богдан Вікторович (Іпс-32)  
**Task:** Blocking Request, Async, Unix Socket, C++  

This project implements a blocking server that processes computation requests using Unix sockets, with asynchronous request handling.

---

## Key Components

### **Classes for Computation (Strategies)**

#### **Task (Base Class):**  
An abstract class defining a computation task.  
- Contains the virtual method `execute(int x)` implemented by derived classes.

#### **Concrete Implementations:**  
- **SquareTask:** Calculates the square of a number: `x * x`.  
- **CubeTask:** Calculates the cube of a number: `x * x * x`.  
- **ReverseTask:** Reverses the digits of a number (e.g., `123 → 321`).  

---

### **UnixSocketServer (Unix Socket Server):**  

**Purpose:** Implements a server to handle requests via Unix sockets with asynchronous processing.  

#### **Core Methods:**  
- **`start()`:** Launches the server, listening on a specified Unix socket path.  
- **`handleClient(int clientSocket)`:** Processes client requests and executes the relevant computation task.  
- **`addTask(const std::string& name, std::shared_ptr<Task> task)`:** Adds a task to the list of available computations.  

---

## Interface Commands

### **Server Commands:**  
1. **`help`**: Displays the list of available commands.  
2. **`start`**: Starts the Unix socket server to accept requests.  
3. **`exit`**: Terminates the program.  