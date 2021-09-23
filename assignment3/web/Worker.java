package web;


import java.io.*;
import java.net.*;
import java.util.*;

class Worker extends Thread implements HttpConstants {

    final static int BUF_SIZE = 2048;
    static final byte[] EOL = {(byte) '\r', (byte) '\n'};

    /* buffer to use for requests */
    byte[] buffer;
    /* Socket to client we're handling */
    private Socket socket;
    /* WebServer the worker works for */
    WebServer webServer;

    Worker(WebServer webServer, String name) {
        super(name);
        this.webServer = webServer;
        buffer = new byte[BUF_SIZE];
        this.start();
    }

    synchronized void setSocket(Socket socket) {
        this.socket = socket;
        notify();
    }

    public synchronized void run() {

        webServer.workerHasStarted();

        while (true) {
            /* nothing to do */
            try {
                wait();
            } catch (InterruptedException e) {
                /* should not happen */
                continue;
            }

            try {
                handleClient();
            } catch (Exception e) {
                e.printStackTrace();
            }
            /* go back in wait queue if there's fewer
             * than numHandler connections.
             */

            Vector pool = webServer.threads;
            synchronized (pool) {
                if (pool.size() >= webServer.workers) {
                    /* too many threads, exit this one */
                    return;
                } else {
                    pool.addElement(this);
                }
            }
        }
    }

    void handleClient() throws IOException {
        InputStream is = new BufferedInputStream(socket.getInputStream());
        PrintStream ps = new PrintStream(socket.getOutputStream());
        /* we will only block in read for this many milliseconds
         * before we fail with java.io.InterruptedIOException,
         * at which point we will abandon the connection.
         */
        socket.setSoTimeout(webServer.timeout);
        socket.setTcpNoDelay(true);

         int number = (int)is.readByte();
         int step = 0;
         int tempNumber;

         if(number == 1)
         {

         }
         else
         {
            tempNumber = (3 * number) + 1;
            steps++;
            while(tempNumber <= 1)
            {
               if(tempNumber % 2 == 0)
               {
                  tempNumber /= 2;
               }
               else
               {
                  tempNumber = (3 * tempNumber) + 1;
               }
            }
         }

         ps.writeByte((char)steps);

         socket.close();
    }

}
