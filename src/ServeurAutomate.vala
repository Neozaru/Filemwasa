extern int wait_for_client(int port);


public class ServeurAutomate : Automate {

	public ServeurAutomate() {

		print("ClientAutomate ctor\n");

		Etat s0 = new Etat("s0 - Standby");
		Etat s1 = new Etat("s1 - Waiting handshake ack");
		Etat s2 = new Etat("s2 - Ready");
		Etat s3 = new Etat("s3 - Waiting data ack");

		Transition t01 = new Transition(s0,s1,"?begin","t01 - Receive hello");
		Transition t10 = new Transition(s1,s0,"!reject","t10 - Accept Abort transaction");
		Transition t12 = new Transition(s1,s2,"!ack","t12 - Send handshake ack");
		Transition t23 = new Transition(s2,s3,"?data","t23 - Receive data");
		Transition t32 = new Transition(s3,s2,"!dack","t32 - Send data ack");
		Transition t30 = new Transition(s3,s0,"?end","t30 - Accept Finish transaction");


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

	public override void init() {
		print("Waiting for client request\n");
		wait_for_client(9009);

	}

	public void wait_message() {
		string message = recv_message();
		print("Received : "+message);

	}



	~ServeurAutomate() {

	}

}