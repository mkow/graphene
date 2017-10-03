import java.io.IOException;
import java.io.OutputStream;
import java.net.InetSocketAddress;

import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpHandler;
import com.sun.net.httpserver.HttpServer;

/*
 * a simple static http server
*/
public class SimpleHttpServer {
  public static HttpServer server;

  public static void main(String[] args) throws Exception {
    server = HttpServer.create(new InetSocketAddress(8000), 0);
    System.out.println("Server listening on 0.0.0.0:8000");
    server.createContext("/test", new MyHandler());
    server.setExecutor(null); // creates a default executor
    Runtime.getRuntime().addShutdownHook(new Thread() {
        @Override
        public void run() {
            System.out.println("Shutting down server ...");
            SimpleHttpServer.server.stop(1);
        }
    });
    server.start();
  }

  static class MyHandler implements HttpHandler {
    public void handle(HttpExchange t) throws IOException {
      byte [] response = "Welcome Real's HowTo test page".getBytes();
      t.sendResponseHeaders(200, response.length);
      OutputStream os = t.getResponseBody();
      os.write(response);
      os.close();
    }
  }
}
