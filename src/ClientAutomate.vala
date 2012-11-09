extern int connect_to_server( string server_name, int port );

public class ClientAutomate : Automate {

	public ClientAutomate() {

		Etat s0 = new Etat("s0 - Standby");
		Etat s1 = new Etat("s1 - Waiting handshake ack");
		Etat s2 = new Etat("s2 - Ready to send data or end stream");
		Etat s3 = new Etat("s3 - Waiting data ack");

		Transition t01 = new Transition(s0,s1,"!begin","t01 - Send Hello");
		Transition t10 = new Transition(s1,s0,"?reject","t10 - Receive transaction abort");
		Transition t12 = new Transition(s1,s2,"?ack","t12 - Receive Hello ACK");
		Transition t23 = new Transition(s2,s3,"!data","t23 - Send data");
		Transition t32 = new Transition(s3,s2,"?dack","t32 - Receive data ACK");
		Transition t20 = new Transition(s2,s0,"!end","t20 - Finish transaction");


		s0.ajouter_transition( t01 );
		s1.ajouter_transition( t12 );
		s1.ajouter_transition( t10 );
		s2.ajouter_transition( t23 );
		s2.ajouter_transition( t20 );
		s3.ajouter_transition( t32 );


		this._etats.append( s0 );
		this._etats.append( s1 );
		this._etats.append( s2 );
		this._etats.append( s3 );


		this.first_state();

	}

	public override void init() {

		string server = "localhost";
		int port = 9009;

		if ( connect_to_server( server, port ) == 0 ) {
			print("I'm now connected ! You can control me.\n");
		}
		else {
			print("Aborted. (is serveur Automate online ?)\n");
			exit_program(1);
		}

	}

	~ClientAutomate() {
		
	}

}