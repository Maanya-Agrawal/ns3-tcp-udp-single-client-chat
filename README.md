# ns-3 TCP and UDP Single Client Communication

## Overview

This project demonstrates **single client-server communication** using both **TCP and UDP protocols** in the ns-3 Network Simulator.

A simple point-to-point network is created where a client sends packets to a server, and the server receives and logs the data.

---

## Concepts Used

* TCP (Transmission Control Protocol)
* UDP (User Datagram Protocol)
* Socket Programming in ns-3
* Packet Transmission and Reception
* Network Simulation

---

## Network Configuration

* **Nodes:** 2

  * Node 0 → Client
  * Node 1 → Server

* **IP Addresses:**

  * Client → 10.1.1.1
  * Server → 10.1.1.2

* **Data Rate:** 5 Mbps

* **Delay:** 2 ms

---

## Files

* `tcp-single-client.cc` → TCP communication
* `udp-single-client.cc` → UDP communication

---

## How to Run

```bash
cd ns-3-dev

# Run UDP
./ns3 run scratch/udp-single-client.cc

# Run TCP
./ns3 run scratch/tcp-single-client.cc
```

---

## Sample Output

### UDP Output

```
Client IP : 10.1.1.1
Server IP : 10.1.1.2
Packet size : 1024 bytes
Send interval : 1s

[Client] Sent 1024 bytes at time 1s
[Server] Received 1024 bytes from 10.1.1.1 at time 1.00369s
...

Total packets sent : 9
Total bytes sent : 9216
```

---

### TCP Output

```
Client IP : 10.1.1.1
Server IP : 10.1.1.2

Total bytes received by server : 5106024
```

---

## TCP vs UDP

| Feature       | TCP                 | UDP            |
| ------------- | ------------------- | -------------- |
| Connection    | Connection-oriented | Connectionless |
| Reliability   | Reliable            | Unreliable     |
| Speed         | Slower              | Faster         |
| Data Delivery | Guaranteed          | Not guaranteed |

---

## Conclusion

This experiment demonstrates how TCP and UDP differ in terms of reliability and performance.
UDP provides faster communication with no guarantee of delivery, while TCP ensures reliable data transfer with higher overhead.

---

## Author

**Maanya Agrawal**

