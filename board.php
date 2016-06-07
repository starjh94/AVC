<?php
include 'dbcon.php'
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
  <h1>게시판 </h1>
  - 게시판 입니다.
  <table>
 <tr>
  <td width = "80">
   글 번호
  </td>
  <td width = "300">
   글 제목
  </td> 
  <td width = "80">
   글쓴이
  </td>
 </tr>
 <?php
  $query = "SELECT * FROM board ORDER BY number DESC";
  $result = mysql_query($query);
  while($row = mysql_fetch_array($result))
  {
   $number = $row['number'];
   $title = $row['title'];
   $name = $row['name'];
   echo("<tr><td>$number</td><td><a href = 'http://localhost/homepage/board/boardRead.php?number=$number'>$title</a></td><td>$name</td></tr>");
  }
 ?>
 <form method = "post" action = "boardWrite.html">
  <input type = "submit" value = "글쓰기">
 </form>
  </table>
 </BODY>
</HTML>
