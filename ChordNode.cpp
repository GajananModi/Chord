#include "ChordNode.hpp"

using namespace std;

// FingerTableEntry methods
FingerTableEntry::FingerTableEntry(std::string ipAddress, int portNumber, ulli nodeIdentifier) {
    // Initialize the object
    this->ipAddress = ipAddress;
    this->portNumber = portNumber;
    this->nodeIdentifier = nodeIdentifier;
}

std::string FingerTableEntry::getIPAddress() {
    // Fetch the ip address
    return this->ipAddress;
}

int FingerTableEntry::getPortNumber() {
    // Fetch the port number
    return this->portNumber;
}

ulli FingerTableEntry::getNodeIdentifier() {
    // Fetch the node identifier
    return this->nodeIdentifier;
}

void FingerTableEntry::setIPAddress(string value) {
    this->ipAddress = value;
}

void FingerTableEntry::setNodeIdentifier(ulli value) {
    this->nodeIdentifier = value;
}

void FingerTableEntry::setPortNumber(int value) {
    this->portNumber = value;
}

// ChordNode methods
ChordNode::ChordNode(string ipAddress, int portNumber, ulli nodeIdentifier) {
    // Initialize the object
    this->ipAddress = ipAddress;
    this->portNumber = portNumber;
    this->nodeIdentifier = (ulli *) malloc(sizeof(ulli));
    *(this->nodeIdentifier) = nodeIdentifier;
    this->hashTable = new unordered_map<ulli, string>();
    this->fingerTable = new vector<FingerTableEntry *>(m);
    for(int i=0; i<m; i++) {
        (*(this->fingerTable))[i] = new FingerTableEntry(this->ipAddress, this->portNumber, *this->nodeIdentifier);
    }
    this->successorList = new vector<FingerTableEntry *>(r, new FingerTableEntry("", -1, __LONG_LONG_MAX__));
    this->predecessor = NULL;
}

void ChordNode::displayFingerTable() {
    if(this->predecessor == NULL) cout << "You are not a part of any chord ring\n";
    else {
        cout << "Finger table entries:\n";
        for(auto i:*(this->fingerTable)) {
            if(i->getIPAddress() == "") {
                cout << "No entries are present\n";
                break;
            }
            cout << i->getIPAddress() << " " << i->getPortNumber() << " " << i->getNodeIdentifier() << "\n";
        }
    }
}

void ChordNode::displayHashTable() {
    if(this->predecessor == NULL) cout << "You are not a part of any chord ring\n";
    else {
        cout << "Hash table entries:\n";
        if(this->hashTable->size() == 0) {
            cout << "No entries are present\n";
        } else {
            for(auto i:*(this->hashTable)) {
                cout << i.first << " " << i.second << "\n";
            }
        }
    }
}

void ChordNode::displayNodeIdentifier() {
    cout << "Node identifier: " << *nodeIdentifier << "\n";
}

void ChordNode::displayPredecessor() {
    if(this->predecessor == NULL) cout << "You are not a part of any chord ring\n";
    else {
        cout << predecessor->getIPAddress() << " " << predecessor->getPortNumber() << " " << predecessor->getNodeIdentifier() << "\n";
    }
}

void ChordNode::displaySuccessorList() {
    if(this->predecessor == NULL) cout << "You are not a part of any chord ring\n";
    else {
        cout << "Successor table entries:\n";
        for(auto i:*(this->successorList)) {
            if(i->getIPAddress() == "") break;
            cout << i->getIPAddress() << " " << i->getPortNumber() << " " << i->getNodeIdentifier() << "\n";
        }
    }
}

void ChordNode::create() {
    if(this->predecessor != NULL) {
        cout << "You are already part of a chord ring\n";
        return;
    }

    // Create a chord ring here
    // this->fingerTable = new vector<FingerTableEntry *>(m, new FingerTableEntry("", -1, __LONG_LONG_MAX__));
    // this->hashTable = new unordered_map<ulli, string>();
    this->predecessor = new FingerTableEntry(this->ipAddress, this->portNumber, *this->nodeIdentifier);
    // this->successorList = new vector<FingerTableEntry *>(r, new FingerTableEntry("", -1, __LONG_LONG_MAX__));
    (*(this->successorList))[0]->setIPAddress(this->ipAddress);
    (*(this->successorList))[0]->setNodeIdentifier(*this->nodeIdentifier);
    (*(this->successorList))[0]->setPortNumber(this->portNumber);
    cout << "Ring created successfully\n";
}

void ChordNode::join(FingerTableEntry * fte) {
    // Make the fte node join the chord ring
    pair<FingerTableEntry *, bool> result = this->findSuccessor(fte->getNodeIdentifier());
    
    int socket_fd; struct sockaddr_in server_details;
    do{
		socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_fd == -1) perror("Error opening socket");
	} while (socket_fd == -1);

    bzero((char *) &server_details, sizeof(server_details));
	server_details.sin_family = AF_INET;
    server_details.sin_port = htons(result.first->getPortNumber());
    server_details.sin_addr.s_addr = inet_addr(result.first->getIPAddress().c_str());

    string command;

    if(result.second) {
        command = "change_predecessor " + fte->getIPAddress() + " " + to_string(fte->getPortNumber()) + " " + to_string(fte->getNodeIdentifier());

        int socket_fd2; struct sockaddr_in server_details2;
        do{
            socket_fd2 = socket(AF_INET, SOCK_STREAM, 0);
            if (socket_fd2 == -1) perror("Error opening socket");
        } while (socket_fd2 == -1);

        bzero((char *) &server_details2, sizeof(server_details2));
        server_details2.sin_family = AF_INET;
        server_details2.sin_port = htons(fte->getPortNumber());
        server_details2.sin_addr.s_addr = inet_addr(fte->getIPAddress().c_str());

        if(connect(socket_fd2, (struct sockaddr *)&server_details2, sizeof(server_details2)) == -1) { perror("Error 2 connecting with peer"); pthread_exit(NULL); }
        string command2 = "change_successor " + result.first->getIPAddress() + " " + to_string(result.first->getPortNumber()) + " " + to_string(result.first->getNodeIdentifier());
        sendData((char *)command2.c_str(), command2.size(), socket_fd2);

        close(socket_fd2);

        do{
            socket_fd2 = socket(AF_INET, SOCK_STREAM, 0);
            if (socket_fd2 == -1) perror("Error opening socket");
        } while (socket_fd2 == -1);

        if(connect(socket_fd2, (struct sockaddr *)&server_details2, sizeof(server_details2)) == -1) { perror("Error 1 connecting with peer"); pthread_exit(NULL); }
        command2 = "change_predecessor " + this->ipAddress + " " + to_string(this->portNumber) + " " + to_string(*this->nodeIdentifier);
        sendData((char *)command2.c_str(), command2.size(), socket_fd2);

        close(socket_fd2);

        (*this->successorList)[0]->setIPAddress(fte->getIPAddress());
        (*this->successorList)[0]->setPortNumber(fte->getPortNumber());
        (*this->successorList)[0]->setNodeIdentifier(fte->getNodeIdentifier());

    } else {
        command = "join_chord " + fte->getIPAddress() + " " + to_string(fte->getPortNumber()) + " " + to_string(fte->getNodeIdentifier());
    }

    if(connect(socket_fd, (struct sockaddr *)&server_details, sizeof(server_details)) == -1) { perror("Error 3 connecting with peer"); pthread_exit(NULL); }
    sendData((char *)command.c_str(), command.size(), socket_fd);
    close(socket_fd);
}

pair<FingerTableEntry *, bool> ChordNode::findSuccessor(ulli id) {
    // Finding successor of id
    if(id <= *this->nodeIdentifier) {
        id = id + pow(2, m);
    }
    ulli successorIdentifier = this->successorList->at(0)->getNodeIdentifier();
    if(successorIdentifier <= *this->nodeIdentifier) {
        successorIdentifier += pow(2, m);
    }
    if(id > *this->nodeIdentifier && id <= successorIdentifier) {
        return make_pair(this->successorList->at(0), true);
    } else {
        return make_pair(this->closestPrecedingNode(id), false);
    }
}

FingerTableEntry* ChordNode::closestPrecedingNode(ulli id) {
    if(id <= *this->nodeIdentifier) {
        id = id + pow(2, m);
    }
    for(int i=m-1; i>=0; i--) {
        if(this->fingerTable->at(i)->getNodeIdentifier() > *this->nodeIdentifier) {
            // Normal
            if(this->fingerTable->at(i)->getNodeIdentifier() < id) {
                return this->fingerTable->at(i);
            }
        }
        else {
            // Rotation performed
            if(this->fingerTable->at(i)->getNodeIdentifier() + pow(2, m) < id) {
                return this->fingerTable->at(i);
            }
        }
    }
    return new FingerTableEntry(this->ipAddress, this->portNumber, *this->nodeIdentifier);
}

void ChordNode::fixFingers(FingerTableEntry * fte) {
    ulli node_identifier = fte->getNodeIdentifier();
    if(node_identifier <= *this->nodeIdentifier) {
        node_identifier += pow(2, m);
    }
    ulli current_node_identifier;
    for(int i = this->fingerTable->size()-1; i >= 0; i--) {
        if(*this->nodeIdentifier + pow(2, i) <= node_identifier) {
            current_node_identifier = this->fingerTable->at(i)->getNodeIdentifier();
            if(current_node_identifier <= *this->nodeIdentifier) {
                current_node_identifier += pow(2, m);
            }

            if(node_identifier < current_node_identifier) {
                this->fingerTable->at(i)->setIPAddress(fte->getIPAddress());
                this->fingerTable->at(i)->setNodeIdentifier(fte->getNodeIdentifier());
                this->fingerTable->at(i)->setPortNumber(fte->getPortNumber());
            }
        }
    }
}

void ChordNode::insertKey(FingerTableEntry * fte) {
    // Make the key value pair insert in the chord ring. Key = node identifier and value = ip address
    pair<FingerTableEntry *, bool> result = this->findSuccessor(fte->getNodeIdentifier());
    
    int socket_fd; struct sockaddr_in server_details;
    do{
		socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_fd == -1) perror("Error opening socket");
	} while (socket_fd == -1);

    bzero((char *) &server_details, sizeof(server_details));
	server_details.sin_family = AF_INET;
    server_details.sin_port = htons(result.first->getPortNumber());
    server_details.sin_addr.s_addr = inet_addr(result.first->getIPAddress().c_str());

    string command;

    if(result.second) {
        command = "insert_key_final " + to_string(fte->getNodeIdentifier()) + " " + fte->getIPAddress();
    } else {
        command = "insert_key " + to_string(fte->getNodeIdentifier()) + " " + fte->getIPAddress();
    }

    if(connect(socket_fd, (struct sockaddr *)&server_details, sizeof(server_details)) == -1) { perror("Error 4 connecting with peer"); pthread_exit(NULL); }
    sendData((char *)command.c_str(), command.size(), socket_fd);
    close(socket_fd);
}

void ChordNode::searchKey(FingerTableEntry * fte) {
    // Search for the value for a particular key in the chord ring. Key = node identifier, ip address = ip address of the requested host and port number = port number of the requested host
    pair<FingerTableEntry *, bool> result = this->findSuccessor(fte->getNodeIdentifier());
    
    int socket_fd; struct sockaddr_in server_details;
    do{
		socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_fd == -1) perror("Error opening socket");
	} while (socket_fd == -1);

    bzero((char *) &server_details, sizeof(server_details));
	server_details.sin_family = AF_INET;
    server_details.sin_port = htons(result.first->getPortNumber());
    server_details.sin_addr.s_addr = inet_addr(result.first->getIPAddress().c_str());

    string command;

    if(result.second) {
        command = "search_key_final " + to_string(fte->getNodeIdentifier()) + " " + fte->getIPAddress() + " " + to_string(fte->getPortNumber());
    } else {
        command = "search_key " + to_string(fte->getNodeIdentifier()) + " " + fte->getIPAddress() + " " + to_string(fte->getPortNumber());
    }

    if(connect(socket_fd, (struct sockaddr *)&server_details, sizeof(server_details)) == -1) { perror("Error 5 connecting with peer"); pthread_exit(NULL); }
    sendData((char *)command.c_str(), command.size(), socket_fd);
    close(socket_fd);
}

void ChordNode::leaveChord() {
    int socket_fd; struct sockaddr_in server_details;
    for(auto itr = (*this->hashTable).begin(); itr != (*this->hashTable).end(); itr++) {
        do{
            socket_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (socket_fd == -1) perror("Error opening socket");
        } while (socket_fd == -1);

        bzero((char *) &server_details, sizeof(server_details));
        server_details.sin_family = AF_INET;
        server_details.sin_port = htons(this->successorList->at(0)->getPortNumber());
        server_details.sin_addr.s_addr = inet_addr(this->successorList->at(0)->getIPAddress().c_str());

        string command = "insert_key_final " + to_string(itr->first) + " " + itr->second;
        if(connect(socket_fd, (struct sockaddr *)&server_details, sizeof(server_details)) == -1) { perror("Error 6 connecting with peer"); pthread_exit(NULL); }
        sendData((char *)command.c_str(), command.size(), socket_fd);

        close(socket_fd);
    }
    
    this->hashTable = new unordered_map<ulli, string>();
    this->fingerTable = new vector<FingerTableEntry *>(m);
    for(int i=0; i<m; i++) {
        (*(this->fingerTable))[i] = new FingerTableEntry(this->ipAddress, this->portNumber, *this->nodeIdentifier);
    }
    this->successorList = new vector<FingerTableEntry *>(r, new FingerTableEntry("", -1, __LONG_LONG_MAX__));
    this->predecessor = NULL;
}

// Threads

struct thread_arguments_structure {
	char command[256]; ChordNode* c; int dataTransferFD;
};

// Interpret the received command from server
void* interpretCommand(void* thread_arguments) {
    string command (((struct thread_arguments_structure *) thread_arguments)->command);
    ChordNode* c = ((struct thread_arguments_structure *) thread_arguments)->c;
    
    vector<string> result;
    boost::split(result, command, boost::is_any_of(" "));
    
    if(result[0] == "join_chord") {
        if(result.size() != 4) { perror("Error the required parameters are join_chord <ip address> <port number> <node identifier>\n"); exit(0); }
        if(c->predecessor != NULL)
            c->join(new FingerTableEntry(result[1], stoi(result[2]), stoull(result[3])));
    } else if (result[0] == "change_predecessor") {
        if(result.size() != 4) { perror("Error the required parameters are change_predecessor <ip address> <port number> <node identifier>\n"); exit(0); }
        c->predecessor = new FingerTableEntry(result[1], stoi(result[2]), stoull(result[3]));
    } else if (result[0] == "change_successor") {
        if(result.size() != 4) { perror("Error the required parameters are change_successor <ip address> <port number> <node identifier>\n"); exit(0); }
        (*c->successorList)[0]->setIPAddress(result[1]);
        (*c->successorList)[0]->setPortNumber(stoi(result[2]));
        (*c->successorList)[0]->setNodeIdentifier(stoull(result[3]));
    } else if (result[0] == "fix_fingers") {
        if(result.size() != 4) { perror("Error the required parameters are fix_fingers <ip address> <port number> <node identifier>\n"); exit(0); }
        if(result[1]==c->ipAddress && stoi(result[2])==c->portNumber && stoull(result[3])==*c->nodeIdentifier) {
            // Don't do anything (as the heart beat message has come back)
        } else {
            int socket_fd; struct sockaddr_in server_details;
            do {
                socket_fd = socket(AF_INET, SOCK_STREAM, 0);
                if (socket_fd == -1) perror("Error opening socket");
            } while (socket_fd == -1);

            bzero((char *) &server_details, sizeof(server_details));
            server_details.sin_family = AF_INET;
            server_details.sin_port = htons(c->successorList->at(0)->getPortNumber());
            server_details.sin_addr.s_addr = inet_addr(c->successorList->at(0)->getIPAddress().c_str());

            if(connect(socket_fd, (struct sockaddr *)&server_details, sizeof(server_details)) == -1) { perror("Error 7 connecting with peer"); pthread_exit(NULL); }
            sendData((char *)command.c_str(), command.size(), socket_fd);
            close(socket_fd);

            c->fixFingers(new FingerTableEntry(result[1], stoi(result[2]), stoull(result[3])));
            cout << "FingerTable updated successfully using IP Address = " << result[1] << ", Port Number = " << result[2] << " and Node Identifier = " << result[3] << "\n";
        }
    } else if (result[0] == "insert_key") {
        if(result.size() != 3) { perror("Error the required parameters are insert_key <key identifier> <key value>\n"); exit(0); }
        c->insertKey(new FingerTableEntry(result[2], -1, stoull(result[1])));
    } else if (result[0] == "insert_key_final") {
        if(result.size() != 3) { perror("Error the required parameters are insert_key_final <key identifier> <key value>\n"); exit(0); }
        (*c->hashTable)[stoull(result[1])] = result[2];
    } else if (result[0] == "search_key") {
        if(result.size() != 4) { perror("Error the required parameters are search_key <key identifier> <ip address> <port number>\n"); exit(0); }
        c->searchKey(new FingerTableEntry(result[2], stoi(result[3]), stoull(result[1])));
    } else if (result[0] == "search_key_final") {
        if(result.size() != 4) { perror("Error the required parameters are search_key_final <key identifier> <ip address> <port number>\n"); exit(0); }

        string valueToSend = "result_value " + result[1] + " ";
        if((*c->hashTable).count(stoull(result[1])) == 1) {
            valueToSend += (*c->hashTable)[stoull(result[1])];
        } else {
            valueToSend += "Error: try after some time as the fingers are not updated yet";
        }
       
        int socket_fd; struct sockaddr_in server_details;
        do {
            socket_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (socket_fd == -1) perror("Error opening socket");
        } while (socket_fd == -1);

        bzero((char *) &server_details, sizeof(server_details));
        server_details.sin_family = AF_INET;
        server_details.sin_port = htons(stoi(result[3]));
        server_details.sin_addr.s_addr = inet_addr(result[2].c_str());

        if(connect(socket_fd, (struct sockaddr *)&server_details, sizeof(server_details)) == -1) { perror("Error 8 connecting with peer"); pthread_exit(NULL); }
        sendData((char *)valueToSend.c_str(), valueToSend.size(), socket_fd);
        close(socket_fd);

    } else if (result[0] == "result_value") {
        if(result.size() != 3) {
            cout << "Error: While trying to fetch the value of a key. Try after some time as the fingers are not updated yet\n";
        } else {
            cout << "The value for the key identifier = " << result[1] << " is = " << result[2] << "\n";
        }
    } else {
        cout << "Invalid command received at the server end\n";
    }

    close(((struct thread_arguments_structure *) thread_arguments)->dataTransferFD);

    pthread_exit(NULL);
}

// Listening port
void* startListeningPort(void* thread_arguments) {
    int socket_fd, data_transfer_fd, port_number = ((ChordNode *)thread_arguments)->portNumber, number_of_characters;
	unsigned int sockaddr_struct_length; long long int command_size; string command; char buffer[256]; bzero(buffer, 256);

    do{
		socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_fd == -1) perror("Error opening socket");
	} while (socket_fd == -1);

	struct sockaddr_in server_details, client_details;
	bzero((char *) &server_details, sizeof(server_details));
	server_details.sin_family = AF_INET;
	server_details.sin_addr.s_addr = INADDR_ANY;
	server_details.sin_port = htons(port_number);

	if (bind(socket_fd, (struct sockaddr *) &server_details, sizeof(server_details)) == -1) { perror("Error binding"); pthread_exit(NULL); }

    listen(socket_fd, INT_MAX);
	sockaddr_struct_length = sizeof(struct sockaddr_in);

    while(1) {
        data_transfer_fd = accept(socket_fd, (struct sockaddr *) &client_details, &sockaddr_struct_length);
        if(data_transfer_fd == -1) { perror("Error accepting"); pthread_exit(NULL); }

        long long int receiver_size;
        string command(receiveData(receiver_size, data_transfer_fd));

        if(command.find("fix_fingers") == string::npos) {
            cout << "Command Received: " << command << "\n";
        }

        pthread_t interpret_command_thread;
        struct thread_arguments_structure * send_chunk_thread_arg = (struct thread_arguments_structure *)malloc(sizeof(struct thread_arguments_structure));
        send_chunk_thread_arg->dataTransferFD = data_transfer_fd;
        strcpy(send_chunk_thread_arg->command, command.c_str());
        send_chunk_thread_arg->c = (ChordNode *)thread_arguments;
		if(pthread_create(&interpret_command_thread, NULL, interpretCommand, (void *) send_chunk_thread_arg)) { perror("Error creating interpret command thread"); pthread_exit(NULL); }
		pthread_detach(interpret_command_thread);
    }
}

// Fix fingers thread
void* fixFingersThread(void* thread_arguments) {
    ChordNode * c = (ChordNode *)thread_arguments;
    string command = "fix_fingers " + c->ipAddress + " " + to_string(c->portNumber) + " " + to_string(*c->nodeIdentifier);
    int socket_fd; struct sockaddr_in server_details;
    
    while(1) {
        if(c->predecessor != NULL) {
            bzero((char *) &server_details, sizeof(server_details));
            server_details.sin_family = AF_INET;
            server_details.sin_port = htons(c->successorList->at(0)->getPortNumber());
            server_details.sin_addr.s_addr = inet_addr(c->successorList->at(0)->getIPAddress().c_str());

            // cout << "Successor Details: " << c->successorList->at(0)->getIPAddress() << " " << c->successorList->at(0)->getPortNumber() << " " << c->successorList->at(0)->getNodeIdentifier() << "\n";

            do {
                socket_fd = socket(AF_INET, SOCK_STREAM, 0);
                if (socket_fd == -1) perror("Error opening socket");
            } while (socket_fd == -1);

            if(connect(socket_fd, (struct sockaddr *)&server_details, sizeof(server_details)) == -1) { perror("Error 9 connecting with peer"); pthread_exit(NULL); }

            sendData((char *)command.c_str(), command.size(), socket_fd);

            close(socket_fd);

            sleep(30);
        }
    }
    pthread_exit(NULL);
}

// Redistribute keys thread
void* redistKeys(void* thread_arguments) {
    ChordNode * c = (ChordNode *)thread_arguments;
    int socket_fd; struct sockaddr_in server_details; string keyValToSend; ulli predId;
    while(1) {
        if(c->predecessor != NULL) {
            for(auto it = (*c->hashTable).begin(); it != (*c->hashTable).end(); ) {
                predId = (c->predecessor->getNodeIdentifier());
                // if(*c->nodeIdentifier <= it->first && predId <= *c->nodeIdentifier) {
                //     predId += pow(2, m);
                // }

                if( (it->first > *c->nodeIdentifier && predId > 0 && predId <= *c->nodeIdentifier && (predId + pow(2, m) >= it->first) ) || (it->first > *c->nodeIdentifier && predId > *c->nodeIdentifier && it->first <= predId )  ||  (it->first <= *c->nodeIdentifier && predId > 0 && predId <= *c->nodeIdentifier && it->first <= predId) ) {
                // if(it->first <= predId && predId <= *c->nodeIdentifier) {
                    bzero((char *) &server_details, sizeof(server_details));
                    server_details.sin_family = AF_INET;
                    server_details.sin_port = htons(c->predecessor->getPortNumber());
                    server_details.sin_addr.s_addr = inet_addr(c->predecessor->getIPAddress().c_str());

                    do {
                        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
                        if (socket_fd == -1) perror("Error opening socket");
                    } while (socket_fd == -1);

                    keyValToSend = "insert_key_final " + to_string(it->first) + " " + it->second;
                    if(connect(socket_fd, (struct sockaddr *)&server_details, sizeof(server_details)) == -1) { perror("Error 7 connecting with peer"); pthread_exit(NULL); }
                    sendData((char *)keyValToSend.c_str(), keyValToSend.size(), socket_fd);
                    close(socket_fd);
                    (*c->hashTable).erase(it->first);
                    cout << "Redistributing key with key identifier = " << it->first << "\n";
                } else {
                    it++;
                }
            }
        }

        sleep(45);
    }
    pthread_exit(NULL);
}