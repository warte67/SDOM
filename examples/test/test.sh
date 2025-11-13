for d in 9 10 11 12 13 14; do
  SDOM_TODAY="November $d, 2025" ./update_latest_anchor.sh --dry-run | grep '<a id='
done
