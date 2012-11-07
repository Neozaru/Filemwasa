
	//extern void my_c_function();

	delegate int function_lol( int a_param );

public class Launcher {


	public static int main (string[] args) {

		if ( args.length > 1 ) {

			if ( args[1] == "client" ) {
				ClientAutomate ca = new ClientAutomate();
				
				ca.debug();

				Cli c = new Cli(ca);	
				c.start();
			}
			else if ( args[1] == "server" ) {
				ServeurAutomate sa = new ServeurAutomate();

				sa.debug();

				Cli s = new Cli(sa);
				s.start();
			}
		}
		else {
			print("Usage : %s client|server\n",args[0]);
		}



		return 0;
	}

}