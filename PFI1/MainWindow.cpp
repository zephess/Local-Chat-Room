#include "MainWindow.h"
#include "ChatWindow.h"
#include "Timer.h"
#include <stdexcept>
#include <string>
#include "pugixml.hpp"
#include "ChangeUsername.h"
#include <iostream>
#include "Wsa.h"

MainWindow::MainWindow() : Fl_Window(640, 480, "Main Window"),
Timer(1.0),
mainCol(0, 0, w(), h()),
subCol(0, 0, 50, h()),
testBox(0, 0, w(), h()),
hostButton(0, 0, 50, 20),
joinButton(0, 0, 50, 30),
otherButton(0, 0, 50, 30),
changeUsnButton(0, 0, 50, 30),
spacer(0, 0, 10, 10),
spacer2(0, 0, 10, 10),
spacer3(0, 0, 10, 10),
serverInitialised(false),
chw(nullptr),
isClient(true),
receivedMessage(false),
server(nullptr),
client(nullptr),
clientSocket(nullptr)
{
	//Initial constructor, window formatting
	//graphic design is my passion
	add_resizable(mainCol);
	mainCol.type(Fl_Flex::COLUMN);
	mainCol.add_resizable(testBox);
	testBox.size(w(), h());
	mainCol.add_resizable(hostButton);
	mainCol.add_resizable(spacer3);
	mainCol.add_resizable(joinButton);
	mainCol.add_resizable(spacer);
	mainCol.add_resizable(changeUsnButton);
	mainCol.add_resizable(spacer2);
	mainCol.add_resizable(otherButton);
	mainCol.fixed(spacer3, 20);
	mainCol.fixed(spacer, 20);
	mainCol.fixed(spacer2, 20);
	mainCol.margin(10);
	mainCol.fixed(hostButton, 50);
	mainCol.fixed(joinButton, 50);
	spacer3.box(FL_NO_BOX);
	spacer.box(FL_NO_BOX);
	spacer2.box(FL_NO_BOX);
	mainCol.fixed(changeUsnButton, 50);
	mainCol.fixed(otherButton, 50);
	hostButton.label("Host Chatroom");
	joinButton.label("Join Chatroom");
	changeUsnButton.label("Change Username");
	otherButton.label("Close Application");
	hostButton.callback(openChat, this);
	hostButton.callback(startHost, this);
	
	otherButton.callback(closeWindow, this);
	joinButton.callback(openChat, this);
	changeUsnButton.callback(changeUsername, this);
	client = nullptr;
}

MainWindow::~MainWindow() 
{
	//Destructor
	printf("MainWindow destructor called\n");
	//if host, send shutdown message to all clients
	if (!isClient) 
	{
		client->send("SERVER_SHUTDOWN");
	}
	//free up memory
	delete client;
	delete server;	
	delete chw;
	Fl::remove_timeout(MainWindow::handleServer, this);
}

void MainWindow::on_tick(void* userData) 
{
	//tick every 0.1s
	MainWindow* window = (MainWindow*)userData;
	Fl::repeat_timeout(0.1, tick, userData);
}


//start hosting + open server
void MainWindow::startHost(Fl_Widget* w, void* data) 
{
	MainWindow* window = (MainWindow*)data;
	printf("Initialized Winsock\n");

	//create a new server on port 8080
	window->server = new ServerSocket(8080);
	window->serverInitialised = true;
	//open chat window
	window->openChat(w, data);
	//prevent user from trying to open another server on the same port
	window->hostButton.deactivate();
	window->hostButton.label("Server Active");
	//mark the user as the host
	window->isClient = false;
}

//handle server connections
void MainWindow::handleServer(void* data) 
{
	MainWindow* window = (MainWindow*)data;
	//if server is active
	if (window->server != nullptr) 
	{
		//call server handle
		window->server->handleConnections();
	}

	//if a message is received during this tick
	std::string message;
	bool receivedMessage = window->client->receive(message);
	if (receivedMessage) 
	{	
		//send the message to be handled in the chat window
		window->chw->receiveMessage(message);
	}
	//repeat every 0.1s
	Fl::repeat_timeout(0.1, MainWindow::handleServer, data);
}

//button callback
void MainWindow::closeWindow(Fl_Widget* w, void* data) 
{
	//get the window, hide it
	MainWindow* window = (MainWindow*)data;
	window->hide();
}

//open chat callback
void MainWindow::openChat(Fl_Widget* w, void* data) 
{
	MainWindow* window = (MainWindow*)data;

	//make sure not doubling up on clients, can cause issues/crashes with message handling
	delete window->client;
	delete window->chw;

	pugi::xml_document doc;
	//check if username doc exists
	if (!doc.load_file("Usernames.xml")) 
	{
		//if it doesnt, create one
		doc.save_file("Usernames.xml");
	}
	//load the username doc
	pugi::xml_parse_result res = doc.load_file("Usernames.xml");
	if (res.status) 
	{
		//error handling
		printf("could not open doc");
	}

	//navigate doc
	pugi::xml_node root = doc.child("Users");
	pugi::xml_node uN = root.child("Username");
	//initialise a username window
	ChangeUsername* usn = new ChangeUsername;
	//if doc is empty, show the username window
	if (uN.attribute("Name").empty()) 
	{
		usn->show();
	}

	//if its not, initialise clientsocket
	if (!uN.attribute("Name").empty())
	{
		//initialise with ip and port
		window->client = new ClientSocket("127.0.0.1", 8080);
		//initialise chat window with client socket and username, as well as a reference to the main window
		window->chw = new ChatWindow(window->client, uN.attribute("Name").value(), (Fl_Window*)data);
		//start handling connections
		Fl::add_timeout(0.1, handleServer, data);
		//show and hide relevant windows
		window->chw->show();
		window->hide();
	}
}
//username window button callback
void MainWindow::changeUsername(Fl_Widget* w, void* data) 
{
	ChangeUsername* usn = new ChangeUsername;
	usn->show();
}
//server active checks. causes small delay when opening, but prevents user from trying to host a server on a taken port
void MainWindow::isServerActive(const std::string& ip, int port) 
{
	try
	{
		//try to create a temporary client and connect to the server
		ClientSocket testClient(ip.c_str(), port);
		testClient.connect();
		std::cout << "Server is active on " << ip << ":" << port << "\n";
		//if connection succeeds, deactivate the host button
		hostButton.deactivate();
		hostButton.label("Server Active");
		joinButton.activate();
	}
	catch (const std::exception& e) 
	{
		//if connection fails, no server is active, deactivate join button
		std::cout << "No server detected on " << ip << ":" << port << "\n";
		hostButton.activate();
		hostButton.label("Host");
		joinButton.deactivate();
	}
}
//Fl_Window.show() override. does server checks every time the main window is reopened. again prevents user-caused crashes
void MainWindow::show() 
{
	//check the server
	isServerActive("127.0.0.1", 8080);
	//then proceed to show normally
	Fl_Window::show();
}



