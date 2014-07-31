package my.restful.api;

import javax.ws.rs.Consumes;
import javax.ws.rs.GET;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.MediaType;
 
@Path("values")
public class RestfulAPI {
	int sum;
	
	// This method is called if TEXT_PLAIN is requested
	@POST
	@Consumes(MediaType.APPLICATION_FORM_URLENCODED)
	@Produces(MediaType.TEXT_PLAIN)
	public String getValue(String value) {
		sum += Integer.parseInt(value);
		return "Sum: " + sum;
	}
 
	// This method is called if HTML is requested
	@GET
	@Produces(MediaType.TEXT_HTML)
	public String sayHelloInHtml() {
		return "<html> " + "<title>" + "Welcome" + "</title>"
				+ "<body><h1>" + "Test" + "</body></h1>" + "</html> ";
	}
}
