#include<dirent.h>
#include<netinet/in.h>
#include<pthread.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>

#define WEBSITE_NAME "127.0.0.1"
#define ROOT "/home/pepper/Projects/sgps/testing_server"
#define SMAXLENGTH 10000
#define MISSING_GOPHERMAP_WARNING ("iA gophermap entry could not be found for this directory and this entry was automatically generated\t\t" WEBSITE_NAME "\t7000\r\n")

struct String {
    char s[SMAXLENGTH];
    size_t len;
};

struct Flags {
    bool r;
    struct String root;
    bool d;
    struct String domain;
    bool p;
    struct String port;
};

void str_pnd(struct String *str, char *toAdd) {
    size_t i = 0;
    while (toAdd[i]) {
	if (str->len >= sizeof(str->s)-1) {
	    printf("str_pnd ran out of space for target string\n");
	    exit(1);
	}
	str->s[str->len] = toAdd[i];
	str->len++;
	i++;
    }
}

void str_pndc(struct String *str, char c) {
    if (str->len >= sizeof(str->s)-1) {
	printf("str_pndc ran out of space for target string\n");
	exit(1);
    }
    str->s[str->len] = c;
    str->len++;
}

bool str_eq(char *s1, char *s2) {
    if (strcmp(s1, s2) == 0 ) {	
	return true;
    } else {
	return false;
    }
}

void create_gentry(struct String *res, char *type, char *str) {
    str_pnd(res, type);
    str_pnd(res, str);
    str_pnd(res, "\t/");
    str_pnd(res, str);
    str_pnd(res, "\t");
    str_pnd(res, WEBSITE_NAME);
    str_pnd(res, "\t7000\r\n");
}

void getDirCon(struct String *res, DIR *dir) {
    str_pnd(res, MISSING_GOPHERMAP_WARNING);
    struct dirent *dirS;
    while ( (dirS = readdir(dir)) != NULL) {
	if (str_eq(dirS->d_name, "gophermap"))
	    continue;
	if (dirS->d_type == DT_REG) {
	    create_gentry(res, "9", dirS->d_name);
	} else if (dirS->d_type == DT_DIR) {
	    create_gentry(res, "1", dirS->d_name);
	}
	printf("The file name is %s\n", dirS->d_name);
    }
}

void getFullDir(struct String *root, struct String *uinput, struct String *path) {
    // UINPUT : "mything/myotherthing/myfile" -> /root/mything/myotherthing/
    // UINPUT : "myfile" -> /root/
    // UINPUT : "" -> /root/
    // I want root +  mything = /some/root/mything

    str_pnd(path, root->s);
    str_pndc(path, '/');

    char *slash = NULL;

    for (char *c = uinput->s; *c; c++) {
	if (*c == '/')
	    slash = c;
    }
    if (slash) {
	char *c = uinput->s;
	while (c != slash) {
	    str_pndc(path, *c);
	    c++;
	}
	str_pndc(path, '/');
    }
    printf("The path is '%s'\n", path->s);
}

void fIntoStr(struct String *str, FILE *f) {
    char c;
    while ((c = getc(f)) != EOF) {
	str_pndc(str, c);
    }
    fclose(f);
}

bool matchLines(struct String *uinput, struct String *line) {
    
    char *t = line->s;
    // advance to the first \t
    while (*t && *t != '\t')
	t++; 
    while (*t && *t == '\t')
	t++; 
    

    struct String toMatch = {0};
    // copty to toMatch until the second \t
    while (*t && *t != '\t') {
	str_pndc(&toMatch, *t);
	t++;
    }
    // check if the entry matched 
    return str_eq(toMatch.s, uinput->s);
}

/* get the filetype for the transaction */
char parseGophermap(struct String *uinput, struct String *pgophermap) {
    // UINPUT : "SubDir/someFile.txt"
    // PGOPHERMAP : 	"1Some Dir\tSomeDir\texample.com\t70\r\n
    // 			 0Some File\tSubDir/someFile.txt\example.com\t70r\n
    // FTYPE : "0" <- gets assigned the type identifier
    /* suported types */

    char *c = pgophermap->s;

    // get a line
    // search the line for uinput
    // assign the first char in line to ftype
    
    char ftype = '\0';
    while (*c) {
	struct String line = {0};
	while (*c && *(c+1) && !(*c == '\r' && *(c+1) == '\n')) {
	    str_pndc(&line, *c);
	    c++;
	}
	// process line
	printf("parsed line: %s\n", line.s);
	if (*c == '\r')
	    c++;
	if (*c == '\n')
	    c++;

	if (matchLines(uinput, &line)) {
	    ftype = line.s[0];
	    printf("Found the entry in the gophermap\n");
	    printf("The entry type is : %c\n", ftype);
	} else {
	    printf("Entry not found in the gophermap\n");
	}
    }
    return ftype;
}

void getdgmapPath(struct String *root, struct String *uinput, struct String *dgmap){
    str_pnd(dgmap, root->s);
    str_pnd(dgmap, "/");
    str_pnd(dgmap, uinput->s);
    str_pnd(dgmap, "/gophermap");
}

void menuTransaction(struct String *response, struct String *dgmap) {
    FILE *map = fopen(dgmap->s, "r");
    
    if (map != NULL) {
	printf("Opening %s for writting to the response\n", dgmap->s);
	fIntoStr(response, map);
    } else {
	printf("File %s failed to open\n", dgmap->s);
	str_pnd(response, "3Menu transaction failed\r\n");
    }
}

void getFilePath(struct String *path, struct String *uinput, struct String *filePath) {
    
    str_pnd(filePath, path->s);
    str_pnd(filePath, uinput->s);
    printf("The file path is %s\n", filePath->s);

}

void textFileTransaction(struct String *response, struct String *filePath) {
    
    FILE *f = fopen(filePath->s, "r");

    if (f != NULL) {
	fIntoStr(response, f);
	printf("Writting the file %s to the response\n", filePath->s);
    } else {
	printf("The file %s could not be opened\n", filePath->s);
	str_pnd(response, "3Error getting the file\r\n");
    }
}

/* container for thread arguments */
struct Args {
    int *c;
    struct Flags *f;
};

void *handle_client(void *ball) {
    /* unpack arguments from void pointer */
    struct Args *fargs = ball;
    int client_fd = *((int *)fargs->c);
    struct Flags flags = *((struct Flags *)fargs->f);

    char inputb[10000];

    read(client_fd, inputb, sizeof(inputb));

    inputb[sizeof(inputb) - 1] = '\0';
    
    char *c = inputb;
    while (*c && *(c+1) && !(*c == '\r' && *(c+1) == '\n')) {
	c++;
    }
    *c = '\0';

    /* client input  */
    struct String uinput = {0};
    str_pnd(&uinput, inputb);
    
    // debugging
    printf("the user sent:\n%s\n", uinput.s);

    /* root of the server */
    struct String root = {0};
    str_pnd(&root, ROOT);

    /* 
    * full dir path of the requested object
    * (i.e. if the user requested /subDir/someThing,
    * the path will be /some/root/subDir)
    */
    struct String path = {0};
    getFullDir(&root, &uinput, &path);
    
    DIR *dir = opendir(path.s);

    
    /* compose response from server */
    struct String response = {0};

    bool dotTerm = true;

    if (dir == NULL) {
	printf("The resquested media was on a directory that wasn't found\n");
	str_pnd(&response, "3The resquested media was not found\r\n");
	goto terminate_con;
    } 

    /* gophermap path, should look like /root/subdirIfAny/gophermap */
    struct String gmapPath = {0};
    str_pnd(&gmapPath, path.s);
    
    /* full path for the fophermap */
    str_pnd(&gmapPath, "gophermap");

    /* gophermap to parse */
    FILE *fgophermap = fopen(gmapPath.s, "r");
    
    struct String pgophermap = {0};

    /* if no gophermap is found for the parent directory, create it at runtime */
    if (fgophermap == NULL) {
	printf("A gophermap wasn't found in the directory of the requested ressource\n");
	/* create a gophermap for the directory of the requested media */
	getDirCon(&pgophermap, dir);
    } else {
	/* write gophermap into string */
	fIntoStr(&pgophermap, fgophermap);
    }
    /* at the end, there must be a gophermap in the pgophermap string */
    printf("The gophermap for the requested media is\n%s\n", pgophermap.s);
	
    /*
    * if the client sends carriage return followed by
    * a line feed, send the contents of the root menu
    */
    
    if (uinput.len == 0) {
	printf("The client requested the root dir\n");
	str_pnd(&response, pgophermap.s);
	goto terminate_con;
    }

    /* get the filetype from the gophermap */
    const char ftype = parseGophermap(&uinput, &pgophermap);

    if (ftype) {
	struct String targetPath = {0};
	switch (ftype) {
	    case '0' :
	    case '6' :
		printf("The requested media is a plain, or uuencoded, starting TextFile Transaction");
		getFilePath(&path, &uinput, &targetPath);
		textFileTransaction(&response, &targetPath);
		break;
	    case '1' :
		printf("The requested media is a directory, starting Menu Transaction\n");
		getdgmapPath(&root, &uinput, &targetPath);
		printf("The path to get is %s\n", targetPath.s);
		menuTransaction(&response, &targetPath);
		break;
	    case '4' :
	    case '5' :
	    case '9' :
	    case 'I' :
	    case 'g' :
	    case 'd' :
		printf("The requested media is a binary file or archive, sending it\n");
		getFilePath(&path, &uinput, &targetPath);
		textFileTransaction(&response, &targetPath);
		dotTerm = false;
		break;
	    default :
		printf("The requested media wasn't supported\n");
		str_pnd(&response, "3Error, the requested media wasn't supported\r\n");
		break;
	}
    } else {
	char *e404 = "3The requested media was not found\r\n";
	str_pnd(&response, e404);
    }

terminate_con:
    //printf("Before the dot, the string constains %s\n", response.s);
    /* append terminator */
    if (dotTerm)
	str_pnd(&response, ".\r\n");

    printf("After the dot, the string constains %s\n", response.s);

    write(client_fd, response.s, response.len);
    shutdown(client_fd, SHUT_WR);
    close(client_fd);
    free(ball);

    return NULL;
}

void checkGopherMaps(void) {
    // TODO check that each directory in the server has a gophermap
}

void usage(void) {
    printf("Usage: sgps [OPTIONS] \n");
    printf("-r [...] : Root of the server\n");
    printf("-d [...] : Domain of the server\n");
    printf("-p [...] : Port to serve from\n");
    printf("Ex : sgps -r /srv -d example.com -p 7070\n");
}

bool checkFlags(struct Flags *flags, int argc, char *argv[]){
    bool odd = false;
    // each flag need an argument
    if (argc % 2 != 0)
	odd = true;
    // get flags and arguments
    for (int i = 1; i < argc; i += 2) {
	// if the flags are not two chars return immediately
	if (strlen(argv[i]) != 2) {
	    printf("The flags must be like this : -r\n");
	    return false;
	}
	// set options for flags
	switch (argv[i][1]) {
	    case 'r' :
		flags->r = true;
		str_pnd(&flags->root, argv[i + 1]);
		break;
	    case 'd' :
		flags->d = true;
		str_pnd(&flags->domain, argv[i + 1]);
		break;
	    case 'p' :
		flags->p = true;
		str_pnd(&flags->port, argv[i + 1]);
		break;
	    default :
		return false;
	}
    }
    // the root is mandatory
    if (!flags->r)
	printf("You must specify a root for the server to serve from.\n");
    // the domain is mandatory
    if (!flags->d)
	printf("You must specify a domain for the server.\n");

    return odd && flags->r && flags->d;
}

int main(int argc, char *argv[]) {
    
    struct Flags flags = {0};

    /* exit imediately if the flags are flagrantly wrong */
    if (!checkFlags(&flags, argc, argv)) {
	usage();
	exit(1);
    }

    /* check that each dir has a gophermap */
    // checkGopherMaps();
    
    /* default port as per the spec */
    int port = 70;
    
    /* if the port is defined, check that it is correct */
    if (flags.p) {
	/* if it's not, exit */
	if ((sscanf(flags.port.s, "%i", &port) != 1) || !(port >= 0 && port <= 65535)) {
	    printf("The port is wrong, dingus.\n");
	    usage();
	    exit(1);
	}
    /* if it is not defined, use the default one */
    } else {
	printf("Port not defined, using port 70\n");
	str_pnd(&flags.port, "70");
    }

    /* check that the root of the server exists on the filesystem */
    if (opendir(flags.root.s) == NULL) {
	printf("The root for the server is not valid.\n");
	usage();
	exit(1);
    } else {
	printf("Serving from : %s\n", flags.root.s);
    }

    printf("The domain is : %s\n", flags.domain.s);
    
    int sock;

    /* create socket ipv4 tcp socket*/

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	perror("Error creating the socket");
	return 1;
    } 
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
	perror("setsockopt(SO_REUSEADDR) failed");

    /* configure socket */
    
    struct sockaddr_in serv_addr;
    /* ipv4 */
    serv_addr.sin_family = AF_INET; 
    /* use any network interface */
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    /* use network byte order on the port */
    serv_addr.sin_port = htons(port);

    /* bind the socket to the port */
    if (bind(sock,
	    (struct sockaddr *)&serv_addr,
	    sizeof(serv_addr)) < 0) {
	perror("Failed to bind socket to port");
	return 1;
    }
    
    /* max number of connections in the queue */
    const int maxcon = 10;
    /* listen on port */
    if (listen(sock, maxcon) < 0) {
	perror("listen on the socket failed");
	return 1;
    }
    printf("Listening for connections on port %d\n", port);

    while(1) {
	/* client info */	
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int *client_fd = malloc(sizeof(int));

	/* accept connection from client */
	if ((*client_fd = accept(sock,
				(struct sockaddr *)&client_addr,
				&client_addr_len)) < 0) {
	    perror("Failled to accept");
	    /* skip thread creation */
	    continue;
	}

	/* prepare struct for passing to the thread */
	struct Args ball = {0};
	ball.c = client_fd;
	ball.f = &flags;

	void *ballp = &ball;
	
	/* create thread for the client request */
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, handle_client, (void *)ballp);
	pthread_detach(thread_id);
	//handle_client((void *) client_fd);
    }
    
    close(sock);

    return 0;
}
