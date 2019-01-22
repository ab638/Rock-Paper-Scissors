#include <gtk/gtk.h>
#include <iostream> // cout
#include <cstring>	// memset
#include <cstdio>	// BUFSIZ
#include <sys/types.h>
#include <string>
#include <sys/socket.h> // socket
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h> // struct timeval
#include <sys/ioctl.h>
#include <unistd.h>
#include <netdb.h>
#include <algorithm> // transform
#include <cstdlib>	// exit
#include <errno.h>	// errno

// globals
GtkBuilder *builder; // builder for glade file
// entry readers
GtkWidget *ipEntry;  GtkWidget *portEntry;
// buttons
GtkWidget *connectButton; GtkWidget *sendButton;
GtkWidget *exitButton;
// radobuttons
GtkWidget *rockRadioButton;
GtkWidget *paperRadioButton;
GtkWidget *scissorsRadioButton;
GtkWidget *exitRadioButton;
// For Text View
GtkWidget *outputBox;
GtkTextBuffer *buffer;
GtkTextBuffer *tmp;

using namespace std;

int sockfd;  // server filedescriptor
string text; // to text view
struct netData {
	const gchar* ip;
	const gchar* port;
};

extern "C" // because I'm not using gtkmm, this allows for the use of the
// gtk's signal handlers and gets rid of the -rdynamic runtime warning
// called when window is closed

int connect_to_server(const gchar* ip, const gchar* port) {
	int sockfd;
	struct addrinfo *r;
	int result;
	string sent;

	getaddrinfo(ip, port, NULL, &r);
	// create a socket for the client
	sockfd = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
	// Connect this socket to the server socket
	if ((result = connect(sockfd, r->ai_addr, r->ai_addrlen)) == -1) {
		return -1;
	}
	return sockfd;
}

void on_window_main_destroy() {
	gtk_main_quit();
}

static void connectButton_clicked_cb(GtkWidget *widget, gpointer* userData) {
	struct netData* y = new struct netData;
	char num[BUFSIZ + 1];
	y->ip = gtk_entry_get_text((GtkEntry *)ipEntry);
	y->port = gtk_entry_get_text((GtkEntry *)portEntry);
	gboolean n_editable = false;

	sockfd = connect_to_server(y->ip, y->port);
	if (sockfd == -1) {
		gtk_text_buffer_set_text(buffer, "Player: connect failed", -1);
	}
	else {
		// disable the entry fields
		gtk_editable_set_editable((GtkEditable *)ipEntry, n_editable);
		gtk_editable_set_editable((GtkEditable *)portEntry, n_editable);
		// disable the connect button
		gtk_widget_set_sensitive(connectButton, false);
		int buflen = BUFSIZ + 1;
		int nread = read(sockfd, num, buflen);
		num[nread + 1] = 0;

		sprintf((char*)text.c_str(), "Welcome to the game!\nYou're player %s.\nPlease make your choice!\n", num);
		gtk_text_buffer_set_text(buffer, (char*) text.c_str(), -1);
	}
	gtk_text_view_set_buffer((GtkTextView *)outputBox, buffer);
	text.clear();
}

static void sendButton_clicked_cb(GtkWidget *widget, gpointer* userData) {
	char buf[BUFSIZ + 1];
	string sent;
	int nread;
	while (1) {
		write(sockfd, "READY", 6);
		nread = read(sockfd, buf, BUFSIZ);
		buf[nread] = 0;
		if (strcmp(buf, "GO") == 0)
			break;
	}
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rockRadioButton))) {
		sent = "Rock";
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(paperRadioButton))) {
		sent = "Paper";
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(scissorsRadioButton))) {
		sent = "Scissors";
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(exitRadioButton))) {
		sent = "STOP";
	}

	write(sockfd, sent.c_str(), sent.length() + 1);
	if (sent == "STOP") {
		text.clear();
		while (1) {
			nread = read(sockfd, buf, BUFSIZ + 1);
			text.append(buf, nread);
			if (nread == 0)
				break;
		}
		gtk_text_buffer_set_text(buffer, (char*) text.substr(5).c_str(), -1);
		gtk_text_view_set_buffer((GtkTextView *)outputBox, buffer);
		gtk_widget_set_sensitive(sendButton, false);
		gtk_widget_set_sensitive(exitButton, true);
		gtk_widget_set_sensitive(connectButton, true);
		close(sockfd);
	}
	else {
		text.clear();
		nread = read(sockfd, buf, BUFSIZ + 1);
		buf[nread] = 0;
		text.append(buf, nread);
		if (strstr(buf, "STOP") != NULL) {
			gtk_text_buffer_set_text(buffer, (char *) text.substr(5).c_str(), -1);
			gtk_text_view_set_buffer((GtkTextView *)outputBox, buffer);
			gtk_widget_set_sensitive(sendButton, false);
			gtk_widget_set_sensitive(exitButton, true);
			gtk_widget_set_sensitive(connectButton, true);
			close(sockfd);
		}
		else {
			gtk_text_buffer_set_text(buffer, (char *) text.c_str(), -1);
			gtk_text_view_set_buffer((GtkTextView *)outputBox, buffer);
		}
	}
}
static void exitButton_clicked_cb(GtkWidget *widget, gpointer* userData) {
	gtk_main_quit();
}

int main (int argc, char *argv[])
{
	GtkWidget *window;


	gtk_init(&argc, &argv);

	// initialize the RPS gui from glade
	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "RPSGUI.glade", NULL);

	// connect the window and the signal
	window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
	g_signal_connect(window, "destroy", G_CALLBACK(on_window_main_destroy), NULL);
	// connect the button and the handler
	connectButton = GTK_WIDGET(gtk_builder_get_object(builder, "connectButton"));
	g_signal_connect(connectButton, "clicked", G_CALLBACK(connectButton_clicked_cb), NULL);
	// connect the entry boxes
	ipEntry = GTK_WIDGET(gtk_builder_get_object(builder, "ipEntry"));
	portEntry = GTK_WIDGET(gtk_builder_get_object(builder, "portEntry"));

	// connect the textview box
	outputBox = GTK_WIDGET(gtk_builder_get_object(builder, "outputBox"));
	buffer = gtk_text_buffer_new(NULL);

	// connect the send button
	sendButton = GTK_WIDGET(gtk_builder_get_object(builder, "sendButton"));
	g_signal_connect(sendButton, "clicked", G_CALLBACK(sendButton_clicked_cb), NULL);
	// connect exit button
	exitButton = GTK_WIDGET(gtk_builder_get_object(builder, "exitButton"));
	g_signal_connect(exitButton, "clicked", G_CALLBACK(exitButton_clicked_cb), NULL);
	// connect all the radio buttons
	rockRadioButton = GTK_WIDGET(gtk_builder_get_object(builder, "rockRadioButton"));
	paperRadioButton = GTK_WIDGET(gtk_builder_get_object(builder, "paperRadioButton"));
	scissorsRadioButton = GTK_WIDGET(gtk_builder_get_object(builder, "scissorsRadioButton"));
	exitRadioButton = GTK_WIDGET(gtk_builder_get_object(builder, "exitRadioButton"));
	//
	g_object_unref(builder);

	// run the gui
	gtk_widget_show(window);
	gtk_main();

	return 0;
}
