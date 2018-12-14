<!DOCTYPE html>
<html lang="zh">
 <head> 
  <meta charset="UTF-8" /> 
  <title>CloriConf</title> 
  <meta name="viewport" content="width=device-width, initial-scale=1.0" /> 
  <link href="css/bootstrap.min.css" rel="stylesheet" /> 
  <link href="css/bootstrap-responsive.min.css" rel="stylesheet" /> 
  <link href="css/site.css" rel="stylesheet" /> 
  <link href="css/main.css" rel="stylesheet"/>
 </head> 

<?php

$path = $_GET["path"];
$group_id=$_GET["group_id"];

?>
 <body> 
  <div id="alert_div" class="alert alert-success alert-spec" role="alert">
  </div> 
  <div class="container"> 
   <div class="row"> 
    <div class="span9"> 
     <div class="hero-unit"> 
      <h2>cloriConf</h2> 
      <p>Zookeeper Management Center</p> 
     </div> 
     <hr/> 
     <form id="edit_profile" class="form-horizontal" method="POST"> 
      <fieldset> 
       <div class="alert alert-success" role="alert"> 
        <div class="control-group"> 
         <label class="control-label" for="input01">Path</label> 
         <div class="controls"> 
          <input type="text" id="params_path" class="input-xlarge" style="width:90%;" name="path" value="<?php echo $path ? $path : "/" ?>" /> 
          <button type="button" id="check_node" name="check_node" class="btn btn-success" >Check</button> 
         </div> 
        </div> 
        <div class="control-group"> 
         <label class="control-label" for="">GroupId</label> 
         <div class="controls"> 
          <select name="group_id" id="params_group_id" class="input-xlarge"> <option value="default">default</option> </select> 
         </div>
        </div> 
        <div class="control-group"> 
         <label class="control-label" for="textarea">Value</label> 
         <div class="controls"> 
          <textarea name="value" class="input-xlarge" id="params_value" rows="15" style="width:100%"></textarea> 
         </div> 
        </div> 
        <div class="form-actions"> 
         <button type="button" id="modify_node" name="modify_node" class="btn btn-primary">Add/Modify</button>
         <button type="button" id="delete_node" name="delete_node" class="btn btn-danger">Delete</button> 
        </div> 
       </div> 
      </fieldset> 
     </form> 
     <h3>Children Nodes</h3> 
     <table class="table table-bordered table-striped"> 
      <thead> 
       <tr> 
        <th>Path</th> 
        <th>GroupId</th> 
        <th>View</th> 
       </tr> 
      </thead> 
      <tbody id="children" > 
       <tr> 
        <td>-</td> 
        <td>-</td> 
        <td><a href="" class="view-link">View</a> </td> 
       </tr> 
      </tbody> 
     </table> 
     <h3> Parent Node </h3> 
     <table class="table table-bordered table-striped"> 
      <thead> 
       <tr> 
        <th>Path</th> 
        <th>GroupId</th> 
        <th>View</th> 
       </tr> 
      </thead> 
      <tbody id="parent"> 
       <tr> 
        <td>-</td> 
        <td>-</td> 
        <td>-<a href="" class="view-link">View</a> </td> 
       </tr> 
      </tbody> 
     </table> 
     <ul class="pager"> 
      <li class="next"> More Infomation <a href="https://github.com/shpilu/cloriConf" target="_blank" title="github">Github</a></li> 
     </ul> 
    </div> 
   </div> 
  </div> 
  <script src="js/jquery.min.js"></script> 
  <script src="js/bootstrap.min.js"></script> 
  <script src="js/site.js"></script>   
  <script src="js/main.js"></script> 
 </body>
</html>
