
# Generate commands that have been used to injest and create first JSDC 2017 on TAP
# 

echo read PASS

for mode in 2017_05_03_{bright,faint}
do
  echo "## please retrieve $mode.vot.gz and run next commands"

  echo "stilts -classpath /home/mellag/gitjmmc/JMMC-OpenDev/oidb/target/original-libs/postgresql-42.2.16.jar -Djdbc.drivers=org.postgresql.Driver        tpipe  in=jsdc_${mode}.vot.gz  cmd='replacecol raj2000 -units degree hmsToDegrees(raj2000)' cmd='replacecol dej2000 -units degree dmsToDegrees(dej2000)' cmd='delcols \"diamFlag* deletedFlag*\"' omode=tosql host=osug-postgres.osug.fr db=oidb_blue dbtable=jsdc_${mode}  protocol=\"postgresql\" write=create user=jmmc_oidb password=\$PASS " ; 
  echo ; 

  echo  "stilts tpipe in=jsdc_${mode}.vot.gz cmd='rowrange 1 1' cmd='replacecol raj2000 -units degree hmsToDegrees(raj2000)' cmd='replacecol dej2000 -units degree dmsToDegrees(dej2000)' cmd='delcols \"diamFlag* deletedFlag*\"' out=jsdc_${mode}_1row.vot"
  echo ; 
  echo "votable2sql.sh -m 0 -p recno -t jsdc_$mode jsdc_${mode}_1row.vot > jsdc_$mode.sql "; 
  echo ; 
  echo "# Run psql then:"
  echo "alter table jsdc_${mode} add recno integer generated always as identity;"
  echo "\\i jsdc_${mode}.sql;";
  echo "CREATE INDEX jsdc_${mode}_spatial ON jsdc_${mode} USING GIST(spoint(radians(raj2000),radians(dej2000)));" 
  echo -e "\n\n\n"; 
done





echo "# Restart the TAP server and check content"
