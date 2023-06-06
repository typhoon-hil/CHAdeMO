CHAdeMO
Communication between an Electric Vehicle (EV) and Electric Vehicle Supply Equipment (EVSE) based on the CHAdeMO standard

This project focuses on establishing communication between an electric vehicle and a charger, specifically implementing the CHAdeMO standard. The CHAdeMO standard is based on the international standard IEC 61851-24, Annex A, and it serves as the Japanese fast charging standard for DC charging. The communication between the EV and EVSE takes place using the Controller Area Network (CAN) bus protocol, which enables bidirectional serial communication.

The CHAdeMO communication protocol consists of five CAN messages, with three messages dedicated to the vehicle side and two messages for the charging station. These messages are implemented within the Typhoon HIL Control Center software.

Both the charging station and the vehicle have one CAN message each for transmitting charge parameters and reporting errors during the charging process. Additionally, the vehicle has a CAN message that expresses the calculated charge time and the state of charge.

CAN messages do not have explicit addresses but instead carry a unique numeric value ID. This ID controls the message's priority on the bus and serves as an identifier for its content. The CAN bus message specification includes two standards: CAN 2.0A with an 11-bit identifier and CAN 2.0B with a 29-bit identifier. The project utilizes the CAN 2.0A standard. CAN bus messages have a maximum length of 8 data bytes.

In the Typhoon HIL environment, CAN communication is only possible with HIL404, HIL604, and HIL606 devices, each of which has two embedded CAN controllers. These controllers can be accessed physically through a standard 9-pin D-sub connector located at the back of the HIL device.

The CAN Setup component is used to configure the CAN controller settings for the chosen HIL device. In the model, exactly one CAN Setup component must exist when utilizing the CAN protocol. Baud rate and execution rate can be specified for each CAN controller. The CAN Send and CAN Receive components allow for defining message ID, message length, and transmission method (event-based or timer-based). Messages can be defined manually or imported from DBC files using both CAN Send and CAN Receive components.

The nonstop execution loop is implemented within the Advanced C Function block, utilizing the C programming language. This block also allows for importing C++ files (.cpp) and header files (.h). The inputs to this block consist of received messages from the charger to the EV, along with auxiliary variables. The outputs from this block consist of messages sent by the EV to the charger, as well as additional variables monitored during simulation to ensure proper functioning.

The Functions - Output function, executed at the specified execution rate on the HIL device, calls the methods from the chademo.cpp file for execution. In addition to calling methods from the added file, the function also checks for received messages and sets the parameters that need to be sent.

The Initialization function contains initializations of auxiliary variables, inputs, and outputs. It also includes a constructor call for CHADEMO(), which is responsible for initializing ordered variables. The charging process logic for the electric vehicle is implemented in the chademo.cpp file. This file monitors and verifies the values of various parameters, both received and to be sent. Based on these values, the relationship between the electric vehicle and the charging station transitions between different states, such as WAIT_FOR_EVSE_PARAMS and RUNNING (charging process).

In the electrical part of the project, the simulation includes different contactors that can be opened or closed using SCADA or specific inputs. A battery, representing the one found in an electric vehicle, is located in the upper right section. This aspect of the project is based on the standard IEC
