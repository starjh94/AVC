<?php
 include 'dbcon.php';
 $TITLE = $_GET['title'];
 $NAME = $_GET['name'];
 $TEXT = $_GET['text'];
 
 $query = "SELECT * FROM board  ORDER BY number DESC";
 $result = mysql_query($query);
 $row = mysql_fetch_array($result);
 
 $NUMBER = $row['number'];
 $NUMBER++;
 $query = "INSERT INTO board VALUE('$NUMBER','$TITLE','$NAME','$TEXT')";
 mysql_query($query);
?>
<html>
<body>
<script language = "javascript">
window.location="board.php"
</script>
</body>
</html>
