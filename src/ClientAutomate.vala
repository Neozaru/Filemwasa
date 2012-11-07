extern int connect_to_server( string server_name, int port );

public class ClientAutomate : Automate {

	public ClientAutomate() {

		print("ClientAutomate ctor\n");

		Etat s0 = new Etat("s0 - Standby");
		Etat s1 = new Etat("s1 - Waiting handshake ack");
		Etat s2 = new Etat("s2 - Ready");
		Etat s3 = new Etat("s3 - Waiting data ack");

		Transition t01 = new Transition(s0,s1,"!begin","t01 - Send handshake");
		Transition t10 = new Transition(s1,s0,"?reject","t10 - Abord transaction");
		Transition t12 = new Transition(s1,s2,"?ack","t12 - Received handshake ack");
		Transition t23 = new Transition(s2,s3,"!data","t23 - Send data");
		Transition t32 = new Transition(s3,s2,"?dack","t32 - Receive data ack");
		Transition t30 = new Transition(s3,s0,"!end","t30 - Finish transaction");


		s0.ajouter_transition( t01 );
		s1.ajouter_transition( t12 );
		s1.ajouter_transition( t10 );
		s2.ajouter_transition( t23 );
		s3.ajouter_transition( t32 );
		s3.ajouter_transition( t30 );


		this._etats.append( s0 );
		this._etats.append( s1 );
		this._etats.append( s2 );
		this._etats.append( s3 );


		this.first_state();

	}



	public void wait_message() {
		string message = recv_message();
		print("Received : "+message);

	}

	/*** Specific functs ***/
	public void begin( string[] cmd ) {

		string msg = "begin";
		send_message(msg);

	}

	public void data( string[] cmd ) {

	}

	public void end( string[] cmd ) {

	}

	public override void init() {

		string server = "localhost";
		int port = 9009;

		connect_to_server( server, port );

		print("Ready : Waiting for user cmd\n");
	}



	////*** Specific functs ***/

	~ClientAutomate() {
		
	}

}