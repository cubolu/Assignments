<?php

  # jsn.php     Assigment in IPP course
  # Author:     Jakub Lukac
  # E-mail:     xlukac09@stud.fit.vutbr.cz
  # Date:       09-03-2016
  # Testing:    PHP 5.5.9
  #             PHP 5.3.3 (merlin)

  # include XML class to encode data

  require "xmldoc.class.php";

  # parsing script arguments

  $shortopts  = "";
  // substitute unsupported characters with value of this argument
  $shortopts .= "h:";   // -h=subst, implicit value is "-"
  // do NOT generate XML declaration
  $shortopts .= "n";
  // add whole converted XML to root tag named by value of this argument
  $shortopts .= "r:";   // -r=root-element, root-element should be valid XML tag
  // string type will be converted to text element
  $shortopts .= "s";
  // number type will be converted to text element
  $shortopts .= "i";
  // constants true, false, null will be converted to elements <true/>,
  // <false/>, <null/>
  $shortopts .= "l";
  // substitute special characters with entity references
  $shortopts .= "c";
  // add size as atribute to array type
  $shortopts .= "a";
  // to each element in array add its index as atribute
  $shortopts .= "t";

  $longopts  = array(
    "help",
    "input:",       // input file - UTF-8
    "output:",      // output file - UTF-8
    // set tag name to array type, implicit value is "array"
    "array-name:",  // --array-name=array-element, array-element should be valid XML tag
    // set tag name to items in array type, implicit value is "item"
    "item-name:",   // --item-name=item-element, item-element should be valid XML tag
    "array-size",   // same as -a
    "index-items",  // same as -t
    "start:",       // set starting index of -t argument
    "padding",      // add zero padding
    #"flattening",
    #"error-recovery",
  );

  $options = getopt($shortopts, $longopts);
  if ($options == false) {
    exit(1);
  }

  # create new document and set custom options

  $doc = new XMLDocument("1.0", "UTF-8");

  # apply options

  $input_file_name = false;
  $output_file_name = false;

  if (isset($options["help"])) {
    if (count($options) != 1) {
      // --help can't be combined with any other options
      exit(1);
    }
    $help_msg  = "Usage: php jsn.php [OPTION]..." . PHP_EOL;
    $help_msg .= "Convert JSON to XML" . PHP_EOL . PHP_EOL;
    $help_msg .= " --help              print this help and exit" . PHP_EOL;
    $help_msg .= " --input-filename    input file" . PHP_EOL;
    $help_msg .= " --output-filename   output file" . PHP_EOL;
    $help_msg .= " -h=subst            substitute unsupported characters with value of this argument," . PHP_EOL;
    $help_msg .= "                     implicit value is \"-\"" . PHP_EOL;
    $help_msg .= " -n                  do NOT generate XML declaration" . PHP_EOL;
    $help_msg .= " -r=root-element     add to root tag named by value of this argument" . PHP_EOL;
    $help_msg .= " -s                  string type will be converted to text element" . PHP_EOL;
    $help_msg .= " -i                  number type will be converted to text element" . PHP_EOL;
    $help_msg .= " -l                  constants true, false, null will be converted to elements <true/>," . PHP_EOL;
    $help_msg .= "                     <false/>, <null/>" . PHP_EOL;
    $help_msg .= " -c                  substitute special characters with entity references" . PHP_EOL;
    $help_msg .= " --array-name=name   set tag \"name\" to array type, implicit value is \"array\"" . PHP_EOL;
    $help_msg .= " --item-name=name    set tag \"name\" to items in array type, implicit value is \"item\"" . PHP_EOL;
    $help_msg .= " -a, --array-size    add size as atribute to array type" . PHP_EOL;
    $help_msg .= " -t, --index-items   to each element in array add its index as atribute" . PHP_EOL;
    $help_msg .= " --start=n           set starting index of -t argument" . PHP_EOL;
    $help_msg .= " --padding           add zero padding to item index" . PHP_EOL;
    print $help_msg;
    exit(0);
  }

  if (isset($options["input"])) {
    $input_file_name = $options["input"];
  }

  if (isset($options["output"])) {
    $output_file_name = $options["output"];
  }

  if (isset($options["h"])) {
    $doc->subst = $options["h"];
  }

  $doc->gen_header = !isset($options["n"]);

  if (isset($options["r"])) {
    if ($doc->is_valid_elem_name($options["r"])) {
      $doc->root_element = $options["r"];
    } else {
      fwrite(STDERR, "Invalid element name: " . $options["r"] . PHP_EOL);
      exit(50);
    }
  }

  if (isset($options["array-name"])) {
    if ($doc->is_valid_elem_name($options["array-name"])) {
      $doc->array_name = $options["array-name"];
    } else {
      fwrite(STDERR, "Invalid element name: " . $options["array-name"] . PHP_EOL);
      exit(50);
    }
  }

  if (isset($options["item-name"])) {
    if ($doc->is_valid_elem_name($options["item-name"])) {
      $doc->item_name = $options["item-name"];
    } else {
      fwrite(STDERR, "Invalid element name: " . $options["item-name"] . PHP_EOL);
      exit(50);
    }
  }

  $doc->text_str = isset($options["s"]);

  $doc->text_num = isset($options["i"]);

  $doc->text_lit = isset($options["l"]);

  $doc->replace_entities = !isset($options["c"]);

  $doc->add_arr_size = isset($options["a"]) || isset($options["array-size"]);

  $doc->add_arr_index = isset($options["t"]) || isset($options["index-items"]);

  if (isset($options["start"])) {
    if (isset($options["t"]) or isset($options["index-items"])) {
      // TODO only N or 0
      $doc->arr_index = $options["start"];
    } else {
      // using --start without -t or --index-items
      exit(1);
    }
  }

  if (isset($options["padding"])) {
    if (isset($options["t"]) or isset($options["index-items"])) {
      $doc->use_padding = true;
    } else {
      // using --padding without -t or --index-items
      exit(1);
    }
  }

  # options colision
  if (isset($options["a"]) and isset($options["array-size"])) {
    exit(1);
  }
  if (isset($options["t"]) and isset($options["index-items"])) {
    exit(1);
  }
  foreach ($options as $key => $value) {
    if (is_array($value)) {
      // using one option multiple times
      exit(1);
    }
  }

  # read input stream and open output stream

  if ($input_file_name) {
    if (is_dir($input_file_name)) exit(2);
    $input_data = file_get_contents($input_file_name) or exit(2);
  } else {
    $input_data = file_get_contents("php://stdin") or exit(2);
  }

  if ($output_file_name) {
    if (is_dir($input_file_name)) exit(3);
    $output_file = fopen($output_file_name, "w") or exit(3);
  } else {
    $output_file = STDOUT;
  }

  # convert JSON input string to object

  $json = json_decode($input_data);
  if ($json == NULL) {
    // JSON input can't be decoded
    fwrite(STDERR, "Failed to parse json string: " . json_last_error_msg() . PHP_EOL);
    exit(4);
  }

  # convert object to XML

  $xml = $doc->xml_encode($json);

  # write XML to output stream

  if (fwrite($output_file, $xml) == false) {
    fwrite(STDERR, "Cannot write to output file/stream" . PHP_EOL);
    exit(100);
  }

  if ($output_file_name) {
    if (fclose($output_file) == false) {
      fwrite(STDERR, "Cannot close output file" . PHP_EOL);
      exit(100);
    }
  }

?>
