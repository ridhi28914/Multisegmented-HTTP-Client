#include <gtk/gtk.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <vector>
//#include "md5.h"
//#include <process.h>
#define HOST ""
#define PAGE "/"
#define PORT 80
#define USERAGENT "HTMLGET 1.1"
//int NUMTHREAD=2;
#define NUMTHREAD 2
using namespace std;
int create_tcp_socket();
char *get_ip(char *host);
char *build_get_query(char *host, char *page);
char* build_get_query(char *host,char* page,int );
void usage();
void *connection_handler(void*);

struct thread_data
{
    int  thread_id;
    int  arg_c;
    int  start_byte;
    int  sz;
    char **arg_v;
    char *arg_v2[5];
};



static GtkWidget *asset_label;
static GtkWidget *frame;
static GtkWidget *entry_a, *entry_s, *entry_v, *entry_t, *entry_r;
static GtkWidget *label_a, *label_s, *label_v, *label_t, *label_r;
static GtkWidget *window, *result_label, *button;
static GtkWidget *table;

static void entry_Submit(GtkWidget *widget,GtkWidget *entry)
{
    const gchar *a,*s,*v,*t1,*r,*m2,*h,*pa;
    const gchar *g[5];
    char *host,*get,*page;
    int argc;
    h = gtk_entry_get_text(GTK_ENTRY (entry_a));
    pa = gtk_entry_get_text(GTK_ENTRY (entry_s));
    g[0] = gtk_entry_get_text(GTK_ENTRY (entry_t));
    g[1] = gtk_entry_get_text(GTK_ENTRY (entry_v));
    g[2] = gtk_entry_get_text(GTK_ENTRY (entry_r));
//md5 = gtk_entry_get_text(GTK_ENTRY (entry_md5));
    /*
    if(t1!="")
    {
     NUMTHREAD++;
    }
    if(v!="")
    NUMTHREAD++;
    */
    struct thread_data thread_data_array[NUMTHREAD];
    char* argv[5];

    printf("NUMTHREAD value is %d\n", NUMTHREAD);
//    printf ("Result: %s , %s, %s, %s, %s\n", a, s, t1, v, r);

    host=(char*) h;
    page=(char*) pa;
    FILE *oFile;
    oFile = fopen ("mainfile.txt", "w");
    fclose(oFile);

    char *p[5];
    for(int i=0; i<5; i++)
    {
        p[i]=(char *)g[i];
        // arr[i]
    }
    // char *p1=(char*)g[2];
    //char *p2=(char*)g[3];


    //char *p1="mirrors/ubuntu-cdimage/releases/12.04.4/release/SHA1SUMS.gpg";
    //char *p2="mirrors/ubuntu-cdimage/releases/12.04.5/release/SHA1SUMS.gpg";
    //  char *arr[5];
    //arr[0]=p1;
    //arr[1]=p2;
    int flag=1;
    int i;
    pthread_t threads[5];

    argv[1]=host;
    argv[2]=page;
    if(page=="")
        argc=2;
    argc=3;
    if(argc > 2)
    {
        page = argv[2];
    }
    else
        page = "/";
    struct sockaddr_in *remote;
    int sock,tmpres,recv_size;
    char *ip;
    char  server_reply[5000],buf[BUFSIZ+1];

    sock = create_tcp_socket();
    ip = get_ip(host);
    fprintf(stderr, "IP is %s\n", ip);
    remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
    remote->sin_family = AF_INET;
    tmpres = inet_pton(AF_INET, ip, (void *)(&(remote->sin_addr.s_addr)));
    if( tmpres < 0)
    {
        perror("Can't set remote->sin_addr.s_addr");
        exit(1);
    }
    else if(tmpres == 0)
    {
        fprintf(stderr, "%s is not a valid IP address\n", ip);
        exit(1);
    }
    remote->sin_port = htons(PORT);

    if(connect(sock, (struct sockaddr *)remote, sizeof(struct sockaddr)) < 0)
    {
        perror("Could not connect");
        exit(1);
    }


    get=build_get_query(host,page);//"GET /~aruhela/index.html HTTP/1.1\r\nHost:  www.cse.iitd.ernet.in\r\nUser-Agent: HTMLGET 1.1\r\n\r\n";

    fprintf(stderr, "Query is:\n<<START>>\n%s<<END>>\n", get);
    if( send(sock , get , strlen(get) , 0) < 0)
    {
        puts("Send failed");
        exit(1);
    }

    int t;
    memset(buf, 0, sizeof(buf));
    //Receive a reply from the server
    while((recv_size = recv(sock , buf , 5000 , 0)) > 0 )
        t+=recv_size;   //t CONTAINS TOTAL SIZE OF FILE

    cout<<"file size is:"<<t<<endl;
    free(get);

    close(sock);
    int j=0;

    for (i=1; i<=NUMTHREAD; i++)
    {
        printf("NUMTHREAD value is %d\n", NUMTHREAD);
        thread_data_array[i].arg_v2[0]=p[0];
        thread_data_array[i].arg_v2[1]=p[1];
        thread_data_array[i].arg_v2[2]=p[2];
        thread_data_array[i].arg_v2[3]=p[3];
        thread_data_array[i].arg_v2[4]=p[4];
        cout<<"i is"<<i<<endl;
        cout<<thread_data_array[i].arg_v2[0]<<endl;
        cout<<thread_data_array[i].arg_v2[1]<<endl;
        thread_data_array[i].thread_id = i;
        thread_data_array[i].arg_c = argc;
        thread_data_array[i].arg_v = argv;
        thread_data_array[i].sz=t/NUMTHREAD;
        thread_data_array[i].start_byte=j;

        cout<<"i is"<<i<<endl;
        if( pthread_create( &threads[i] , NULL ,  connection_handler , (void *) &thread_data_array[i] )< 0)
        {
            perror("could not create thread");
            //return 1;
        }
        j+=(t/NUMTHREAD);
        cout<<"i is"<<i<<endl;
        // p++;
        // j+=thread_data_array[i].sz;
    }

    cout<<"exited before end"<<endl;

    for(int i=1; i<=NUMTHREAD; i++)
        pthread_join(threads[i],NULL);

    cout<<"after threads"<<endl;
//    close(sock);

    oFile = fopen ("mainfile.txt", "a+");

    for(int i=1; i<=NUMTHREAD; i++)
    {
        cout<<endl<<"here";
        FILE    *infile;
        char    *buffer;
        long    numbytes;

        char filename[50];
        sprintf(filename, "file_data%01d.txt", i);
        cout<<filename<<endl;
        /* open an existing file for reading */
        infile = fopen(filename, "r+");

        /* quit if the file does not exist */
        if(infile == NULL)
        {
            cout<<"no file";
            //return 1;
        }

        /* Get the number of bytes */
        fseek(infile, 0L, SEEK_END);
        numbytes = ftell(infile);

        /* reset the file position indicator to
        the beginning of the file */
        fseek(infile, 0L, SEEK_SET);

        /* grab sufficient memory for the
        buffer to hold the text */
        buffer = (char*)calloc(numbytes, sizeof(char));

        /* memory error */
        if(buffer == NULL)
            // return 1;

            /* copy all the text into the buffer */
            //fread(buffer, sizeof(char), numbytes, infile);
            //fclose(infile);

            /* confirm we have read the file by
            outputing it to the console */
            printf("The file called filedata1.txt contains this text\n\n%s", buffer);


        size_t bytes;

        while (0 < (bytes = fread(buffer, sizeof(char), numbytes, infile)))
            fwrite(buffer, sizeof(char), bytes, oFile);


//fwrite (buffer , sizeof(char*), sizeof(buffer), oFile);

        /* free the memory we used for the buffer */
        free(buffer);
        fclose(infile);
        remove(filename);
    }
    fclose(oFile);

    //   MD5 md5 ;
    //puts( md5.digestFile( "mainfile.txt" ) ) ;

    //pthread_exit(NULL);

}


static void destroy(GtkWidget *widget, gpointer data)
{
    gtk_main_quit ();
}



static void initialize_window(GtkWidget* window)
{
    gtk_window_set_title(GTK_WINDOW(window),"My Window"); //Set window title
    gtk_window_set_default_size (GTK_WINDOW (window), 400, 200); //Set default size for the window
    g_signal_connect (window, "destroy", G_CALLBACK (destroy), NULL); //End application when close button clicked

}

int main (int argc, char *argv[])
{
    //GtkWidget *window,*table,*label, *button;
    gtk_init(&argc, &argv);


    //Create the main window
    //window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    //initialize_window(window);
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (GTK_WIDGET (window), 300, 300);
    gtk_window_set_title (GTK_WINDOW (window), "FINANCIAL CALCULATOR");
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    g_signal_connect_swapped (window, "delete-event", G_CALLBACK (gtk_widget_destroy),window);



    /* Create a 1x2 table */
    table = gtk_grid_new ();
    gtk_container_add (GTK_CONTAINER (window), table);

    //create a text box (asset price)
    entry_a = gtk_entry_new ();
    gtk_grid_attach (GTK_GRID (table), entry_a, 0, 0, 1, 1);

    // create a new label.
    label_a = gtk_label_new (" Host" );
    gtk_grid_attach (GTK_GRID (table), label_a, 1, 0, 1, 1);



    //create a text box (strike price)
    entry_s = gtk_entry_new ();
    gtk_grid_attach (GTK_GRID (table), entry_s, 0, 1, 1, 1);

    // create a new label.
    label_s = gtk_label_new (" Page" );
    gtk_grid_attach (GTK_GRID (table), label_s, 1, 1, 1, 1);



    //create a text box (time to maturity)
    entry_t = gtk_entry_new ();
    gtk_grid_attach (GTK_GRID (table), entry_t, 0, 2, 1, 1);

    // create a new label.
    label_t = gtk_label_new ("mirror1 Page:-" );
    gtk_grid_attach (GTK_GRID (table), label_t, 1, 2, 1, 1);



    //create a text box (volatility)
    entry_v = gtk_entry_new ();
    gtk_grid_attach (GTK_GRID (table), entry_v, 0, 3, 1, 1);

    // create a new label.
    label_v = gtk_label_new ("mirror2 Page:-" );
    gtk_grid_attach (GTK_GRID (table), label_v, 1, 3, 1, 1);



    //create a text box (interest rate)
    entry_r = gtk_entry_new ();
    gtk_grid_attach (GTK_GRID (table), entry_r, 0, 4, 1, 1);

    // create a new label.
    label_r = gtk_label_new ("Md5sum" );
    gtk_grid_attach (GTK_GRID (table), label_r, 1, 4, 1, 1);

    button = gtk_button_new_with_label("Download");
    g_signal_connect_swapped (button, "clicked", G_CALLBACK (entry_Submit), entry_a);

    gtk_grid_attach (GTK_GRID (table), button, 0, 6, 2, 1);

    gtk_widget_show_all(window);

    gtk_main ();
    return 0;
}



void *connection_handler(void *threadarg)
{
    struct thread_data *my_data;
    char *host,*page;
    struct sockaddr_in *remote;
    int tmpres,recv_size,sock;
    char *ip,*get;
    char server_reply[5000], buf[BUFSIZ+1];

    my_data = (struct thread_data *) threadarg;
    cout<<"thread num:"<<my_data->thread_id<<endl;

    //CREATING SOCKET AND CONNECTING AGAIN
    host = my_data->arg_v[1];

    if(my_data->arg_c > 2)
        page = my_data->arg_v[2];
    else
        page = "/";

    page=my_data->arg_v2[(my_data->thread_id) - 1];

    int c=my_data->start_byte;
    int tot=my_data->sz;   //size of chunk to be downloaded

    sock = create_tcp_socket();
    ip = get_ip(host);
    fprintf(stderr, "IP is %s\n", ip);

    remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
    remote->sin_family = AF_INET;
    tmpres = inet_pton(AF_INET, ip, (void *)(&(remote->sin_addr.s_addr)));

    if( tmpres < 0)
    {
        perror("Can't set remote->sin_addr.s_addr");
        exit(1);
    }
    else if(tmpres == 0)
    {
        fprintf(stderr, "%s is not a valid IP address\n", ip);
        exit(1);
    }
    remote->sin_port = htons(PORT);

    if(connect(sock, (struct sockaddr *)remote, sizeof(struct sockaddr)) < 0)
    {
        perror("Could not connect");
        exit(1);
    }

    get = build_get_query(host, page,c);

    fprintf(stderr, "Query is:\n<<START>>\n%s<<END>>\n", get);
    //Send the query to the server
    int sent = 0;

    while(sent < strlen(get))
    {
        tmpres = send(sock, get+sent, strlen(get)-sent, 0);
        if(tmpres == -1)
        {
            perror("Can't send query");
            exit(1);
        }
        sent += tmpres;
    }
    memset(buf, 0, sizeof(buf));
    int htmlstart = 0,ini=0;
    char * htmlcontent;
    tmpres=0;
    char filename[50];
    sprintf(filename, "file_data%01d.txt", my_data->thread_id);
    cout<<filename<<endl;
    FILE * file;
    file= fopen(filename,"a+");

    while(ini<tot)    // till the whole chunk not received
    {
        while((tmpres = recv(sock, buf, BUFSIZ, 0)) > 0)
        {

            ini+=tmpres;   // ADDING BYTES RECEIVED
            if(htmlstart == 0)
            {
                htmlcontent = strstr(buf, "\r\n\r\n");
                if(htmlcontent != NULL)
                {
                    htmlstart = 1;
                    htmlcontent += 4;
                }
            }
            else
            {
                htmlcontent = buf;
            }
            if(htmlstart)
            {
                fprintf(stdout, htmlcontent);
                fprintf(file, htmlcontent);
            }
            memset(buf, 0, tmpres);
        }

    }
    fclose(file);

    if(tmpres < 0)
    {
        perror("Error receiving data");
    }

    free(get);
    free(remote);
    free(ip);
    close(sock);
    //return 0;
}
void usage()
{
    fprintf(stderr, "USAGE: htmlget host [page]\n\
\thost: the website hostname. ex: coding.debuntu.org\n\
\tpage: the page to retrieve. ex: index.html, default: /\n");
}


int create_tcp_socket()
{
    int sock;
    if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        perror("Can't create TCP socket");
        exit(1);
    }
    return sock;
}


char *get_ip(char *host)
{
    struct hostent *hent;
    int iplen = 15; //XXX.XXX.XXX.XXX
    char *ip = (char *)malloc(iplen+1);
    memset(ip, 0, iplen+1);
    if((hent = gethostbyname(host)) == NULL)
    {
        herror("Can't get IP");
        exit(1);
    }
    if(inet_ntop(AF_INET, (void *)hent->h_addr_list[0], ip, iplen) == NULL)
    {
        perror("Can't resolve host");
        exit(1);
    }
    return ip;
}

char *build_get_query(char *host, char *page,int i)
{
    char *query;
    char *getpage = page;

    int j=i+499;
    char tpl[100] = "GET /%s HTTP/1.1\r\nHost: %s\r\nRange: bytes=%d-%d\r\nUser-Agent: %s\r\n\r\n";

    if(getpage[0] == '/')
    {
        getpage = getpage + 1;
        fprintf(stderr,"Removing leading \"/\", converting %s to %s\n", page, getpage);
    }

    query = (char *)malloc(strlen(host)+strlen(getpage)+8+strlen(USERAGENT)+strlen(tpl)-5);
    sprintf(query, tpl, getpage, host, i , j, USERAGENT);
    return query;
}
char *build_get_query(char *host, char *page)
{
    char *query;
    char *getpage = page;
    //char tpl[1000] = "GET /%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n";
    char tpl[100] = "GET /%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n";
    if(getpage[0] == '/')
    {
        getpage = getpage + 1;
        fprintf(stderr,"Removing leading \"/\", converting %s to %s\n", page, getpage);
    }
    // -5 is to consider the %s %s %s in tpl and the ending \0
    query = (char *)malloc(strlen(host)+strlen(getpage)+strlen(USERAGENT)+strlen(tpl)-5);
    sprintf(query, tpl, getpage, host, USERAGENT);
    return query;
}
