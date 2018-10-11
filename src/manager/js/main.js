$(function() {

    var local_url = "index.php";
    var server_url = "zk_service.php";
    
    $("#check_node").unbind('click').bind('click', "check", function(event){ getConfigList(event.data)});
    $("#modify_node").unbind('click').bind('click', "modify", function(event) { 
        var ret = confirm("Are you sure to update this config");
        if (ret == true) {
            getConfigList(event.data);
        }
    });
    $("#delete_node").unbind('click').bind('click', "delete", function(event) { 
        var ret = confirm("Are you sure to delete this config");
        if (ret == true) {
            getConfigList(event.data);
        }
    });

    getConfigList("check");

    function getConfigList($action) {
        var form = new FormData(document.getElementById("edit_profile"));
        form.append("action", $action);

        $.ajax({
            type:'POST',
            url:server_url,
            timeout:3000,
            contentType:false,
            processData:false,
            async:true,
            data:form,
            dataType:'JSON',
            success:function(res) {
                if (res.errorCode == 200) {
                    render(res.data);
                }
                alertInfo(res.msg);
            },
            error:function() {
                alertInfo("get config info failed");
            }
        });
    }

    // TODO
    function render(data) {
        // { 
        //   "path":"xxx",
        //   "value":"xxx",
        //   "group_id":"xxx",
        //   "children":["xx1","xx2"],
        //   "parent":"xxx"
        // }
        $("#params_path").val(data.path);
        $("#params_value").val(data.value);
        // 
      // <tbody id="parent"> 
      //  <tr> 
      //   <td>-</td> 
      //   <td>-</td> 
      //   <td>-<a href="" class="view-link">View</a> </td> 
      //  </tr> 
        
        var ele_parent = "<tr>";
        ele_parent += "<td>" + data.parent + "</td>";
        ele_parent += "<td>" + data.group_id + "</td>";
        ele_parent += "<td><a href=\"" + local_url + "?group_id=" + data.group_id + "&path=" + data.parent +
          "\" class=\"view_link\">View</a></td></tr>";
        $("#parent").empty().append(ele_parent);
        $("#children").empty();
        if (data.children_nodes.length > 0) {
            var ele_children = "";
            for (var i = 0; i < data.children_nodes.length; ++i) {
                ele_children += "<tr>";
                ele_children += "<td>" + data.children_nodes[i] + "</td>";
                ele_children += "<td>" + data.group_id + "</td>";
                ele_children += "<td><a href=\"" + local_url + "?group_id=" + data.group_id + "&path=" + data.children_nodes[i] +
                  "\" class=\"view_link\">View</a></td></tr>";
            }
            $("#children").append(ele_children);
        }
        
    }

    function alertInfo($msg) {
        $("#alert_div").text($msg).stop().slideDown();
        setTimeout(function(){ 
            $("#alert_div").stop().slideUp();
        }, 2000);
    }
})
