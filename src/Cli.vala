extern int send_message(string message);
extern string recv_message();

public class Cli {
    

  //delegate int Command_Function( string[] args );
  //public struct



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
    return input.str;

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
      return str.strip().split(" ");
    }
    else {
      return new string[0];
    }

  }


  public int exec_cmd() {

    string[] cmd = get_cmd();



    if ( cmd.length > 0 ) {

      print("Command "+cmd[0]+"\n");

        if ( cmd[0] == "exit" ) {
          this.exit(cmd);
        }
        else if ( cmd[0] == "help" ) {
          print("exit : Exit the programm\n");
          print("info : Prints info about Automate\n");
          print("!<message> : Send a message\n");
          print("?<message> : Receive a message\n");
          
        }
        else {
          _automate.on_command(cmd);
        }

      }

    return 0;
  }


  public int exit( string[] cmd ) {
      _running = false;
      print("Exiting ...\n");
      return 0;
  }



  ~Cli() {

  }




} 