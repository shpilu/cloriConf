<?php

function CopyZookeeper($src, $dst) {
    $ZK_HOST = "127.0.0.1:2181";
    $zk = new Zookeeper($ZK_HOST);

    if (!$zk->exists($src)) {
        echo $src . " not exists";
        return;
    }
    $acl = array(
        array(
            'perms' => Zookeeper::PERM_ALL,
            'scheme' => 'world',
            'id' => 'anyone'
        )
    );
    $arr1 = $zk->getChildren($src); 
    while (count($arr1) != 0) {
        $val = $zk->get($src . "/" . $arr1[0]);
        $dst_node = $dst . "/" . $arr1[0];
        echo "creating dst_node=" . $dst_node . "\n";
        $zk->create($dst_node, $val, $acl);
        $children = $zk->getChildren($src . "/" . $arr1[0]);
        foreach ($children as $child) {
            array_push($arr1, $arr1[0] . "/" . $child);
        }
        array_shift($arr1);
    }
    echo "All done";
}

CopyZookeeper("/online/commercial/ssp", "/test/commercial/ssp");

?>
