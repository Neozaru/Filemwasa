
	//extern void my_c_function();

	extern int wait_for_client(int port);
	delegate int function_lol( int a_param );

public class Launcher {


	public static int main (string[] args) {

		string hello = "test";
		print ("%s\n", hello);
		//my_c_function();
		function_lol f1 = wait_for_client;

		//int a_fonction( int ) = &wait_for_client;
		f1(6987);

		ClientAutomate ca = new ClientAutomate();
		ca.debug();

		return 0;
	}

}