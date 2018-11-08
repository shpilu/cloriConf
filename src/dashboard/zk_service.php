<?php

// Zookeeper handler
// version: 1.0 
// Copyright 2018 James Wei (weijianlhp@163.com)

$path = $_POST["path"];
$value = $_POST["value"];
$group_id = $_POST["group_id"];
$action = $_POST["action"];

$path = rtrim($path, "/");
$is_root = false;
if (strlen($path) == 0) {
    $path = "/";
    $is_root = true;
}

$res = new stdClass();
$res->errorCode = 200;
$res->msg = "success";
$data = new stdClass();

$ZK_HOST = "127.0.0.1:2181";
$zk = new Zookeeper($ZK_HOST);

if ($action == "modify") {
    if ($zk->exists($path)) {
        if (!$zk->set($path, $value)) {
            $res->errorCode = 500;
            $res->msg = "set Zookeeper failed";
        } else {
            $res->msg = "modify node success";
        }
    } else {
        if ($zk->exists(dirname($path))) {
            $acl = array(
                array(
                    'perms' => Zookeeper::PERM_ALL,
                    'scheme' => 'world',
                    'id' => 'anyone'
                )
            );
            if (!$zk->create($path, $value, $acl)) {
                $res->errorCode = 505;
                $res->msg = "create Zookeeper node failed";
            } else {
                $res->msg = "create node success";
            }
        } else {
            $res->msg = "cannot create node, as parent nodes do not exist";
        }
    }
} else if ($action == "delete") {
    if ($zk->exists($path)) {
        if (count($zk->getChildren($path)) == 0) {
            $zk->delete($path);
            $path = dirname($path);
            $res->msg = "delete node success";
        } else {
            $res->msg = "cannot delete node, as children nodes exist";
        }
    } else {
        $res->msg = "node do not exist";
    }
}

// check
$data->path = $path;
$data->children_nodes = array();
$data->parent = dirname($path); 

if ($zk->exists($path)) {
    $data->value = $zk->get($path);
    $children = $zk->getChildren($path); 
    if ($is_root) {
        foreach ($children as $child) {
            array_push($data->children_nodes, "/" . $child);
        }
    } else {
        foreach ($children as $child) {
            array_push($data->children_nodes, $path . "/" . $child);
        }
    }
} else {
    $data->value = ""; 
    if ($action == "check") {
        $res->msg = "[node do not exist]path=" . $path;
    }
}

$data->group_id=$group_id;

$res->data = $data;
$json = json_encode($res);
echo $json;

?>
