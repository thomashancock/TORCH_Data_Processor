import sys
import os


def main():
    """
    Main Function
    """

    # Open output to write files to
    with open('filelist.lst','w') as output:
        # Loop through arguments, ignoring the first (which is the script name)
        for arg in sys.argv[1:]:
            print('Reading files from directory: {0}'.format(arg))
            # Write files to output, ignoring subdirectories
            output.write('\n'.join([os.path.join(arg, f) for f in os.listdir(arg) if os.path.isfile(os.path.join(arg, f))]))
            # Add extra newline as join won't print one at the end
            output.write('\n')


if __name__ == '__main__':
    main()
