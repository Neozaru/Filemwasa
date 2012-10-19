public class ClientAutomate : Automate {

	public ClientAutomate() {

		print("ClientAutomate ctor\n");

		Etat s0 = new Etat("s0 - Standby");
		Etat s1 = new Etat("s1 - Waiting handshake ack");
		Etat s2 = new Etat("s2 - Ready");
		Etat s3 = new Etat("s3 - Waiting data ack");

		Transition t01 = new Transition(s0,s1,"","t01 - Send handshake");
		Transition t10 = new Transition(s1,s0,"","t10 - Abord transaction");
		Transition t12 = new Transition(s1,s2,"","t12 - Received handshake ack");
		Transition t23 = new Transition(s2,s3,"","t23 - Send data");
		Transition t32 = new Transition(s3,s2,"","t32 - Receive data ack");
		Transition t30 = new Transition(s3,s0,"","t30 - Finish transaction");


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

	}


	~ClientAutomate() {
		
	}

}