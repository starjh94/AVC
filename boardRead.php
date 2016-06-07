<?php
 include 'dbcon.php';
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML>
 <HEAD>
  <TITLE> New Document </TITLE>
  <META NAME="Generator" CONTENT="EditPlus">
  <META NAME="Author" CONTENT="">
  <META NAME="Keywords" CONTENT="">
  <META NAME="Description" CONTENT="">
 </HEAD>
 <BODY>
 <?php
 $NUMBER = $_GET['number'];
$query = "SELECT * FROM board where number='$NUMBER'";
$result = mysql_query($query);
$row = mysql_fetch_array($result);
$number = $row['number'];
$title = $row['title'];
$name = $row['name'];
$text = $row['text'];
 ?>
 <h1>
 글 읽기
 </h1>
 <table>
 <tr>
  <td>
   글번호 : 
  </td>
  <td>
   <?php echo("$number")?>
  </td>
 </tr>
 <tr>
  <td>
   제목 : 
  </td>
  <td>
    <?php echo("$title")?>
  </td>
 </tr>
 <tr>
  <td>
   글쓴이 : 
  </td>
  <td>
   <?php echo("$name")?>
  </td>
 </tr>
 <tr>
  <td>
   내용 :
  </td>
  <td>
   <?php echo("$text")?>
  </td>
 </tr>
 </table>
  
 </BODY>
</HTML>