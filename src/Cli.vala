extern void init_network();
extern int send_message(string message);
extern string recv_message();
extern void exit_program( int code );


public class Cli {
    
  private Automate _automate;
  private bool _running;

  static string read_stdin( bool single_line ) {

    var input = new StringBuilder ();
    var buffer = new char[1024];
    while (!stdin.eof()) {
      string read_chunk = stdin.gets (buffer);
      if (read_chunk != null) {
        input.append(read_chunk);
        if(single_line) {
          break;
        }
      }
    }

    return input.str.replace("\n","");

  }


  public Cli( Automate auto ) {

    _automate = auto;

  }

  public void start() {

    _automate.init();
    _running = true;

    while ( _running ) {

      exec_cmd();

    }

  }

  public string[] get_cmd() {

    print("> ");
    string str = read_stdin(true);
    if ( str.length > 0 ) {
      string[] cmd = str.split(" ");
      return cmd;
    }
    else {
      return new string[0];
    }

  }


  public int exec_cmd() {

    string[] cmd = get_cmd();

    if ( cmd.length > 0 ) {

        if ( cmd[0] == "exit" ) {
          exit_program(0);
        }
        else if ( cmd[0] == "info" ) {
          _automate.print_current_state();
        }
        else if ( cmd[0] == "help" ) {
          print("--------------------------------\n");
          print("exit : Exits the program\n");
          print("info : Prints info about Automate\n");
          print("!<message> : Send a message\n");
          print("? : Receive a message\n");
          print("--------------------------------\n");
          
        }
        else {
          _automate.on_command(cmd);
        }

      }

    return 0;
  }

  ~Cli() {

  }


} 