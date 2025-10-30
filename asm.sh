dataset=("facebook" "dblp" "flickr" "nethept" "epinions" "youtube" "pokec" "orkut" "livejournal" "friendster" "DBLP_sym" "Youtube_sym" "twitter" "citeseer" "Flickr_sym" "wikitalk" "wikitalkar" "sample" "Twitter")
# data_No=4 # 4: epinions, 8: livejournal, 10: DBLP_sym, 11: Youtube_sym
# i=(1 2 3 4 5)

if [ "$1" -lt 0 ] || [ "$1" -ge ${#dataset[@]} ]; then
    echo "Error: Invalid dataset index"
    exit 1
fi

if ! [[ "$2" =~ ^[0-9]+$ ]]; then
    echo "Error: eta must be a number"
    exit 1
fi

model="IC"
batch=2
eps=0.7
cur_date=$( date +"%m-%d")
eta=$2
set="lj2"
# k=$((((eta-2000)/2000)+1))
# set="${dataset[$1]}${k}"
if [[ "$1" -eq 9 ]]; then
    eps=0.9
    batch=32
fi
if [[ "$1" -eq 11 && "$model" == "IC" ]]; then
    batch=8
fi
if [[ "$1" -eq 4 && "$model" == "LT" ]]; then
    batch=1
fi
if [[ "$1" -eq 8 ]]; then
    if [[ "$model" == "IC" ]]; then
        batch=8
    else
        batch=4
    fi
fi
if [[ "$1" -eq 18 ]]; then
    eps=0.9
    batch=4
fi
# seg=""
# if [ "$1" -eq 4 ]; then
#     set="mcss${k}"
# fi
start_time=5
end_time=6
delta_amp=10
# OUTPUT="log_asti/ASTI_${dataset[$1]}_${eta}_b${batch}_eps${eps}_${start_time}-${time}_${cur_date}.log"
OUTPUT="log_asti/ASTI_${dataset[$1]}_${eta}_b${batch}_eps${eps}_delta_amp${delta_amp}_${start_time}-${end_time}_${model}_${cur_date}.log"
#-start_time "$start_time" -time "$time"
sudo cset proc -s "$set" -e -- ./asm -dataset_No "$1" -eta "$2" -batch "$batch" -delta_amp "$delta_amp" -eps "$eps"  -cost_type "1" -start_time "$start_time" -time "$end_time" -model "$model" | tee -a $OUTPUT